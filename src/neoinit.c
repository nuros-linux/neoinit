#include "neoinit.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <pthread.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>

#define MAX_DEPS 32
#define MAX_EVENTS 64
#define SOCKET_PATH "/run/neoinit.sock"

typedef struct {
    char **deps;
    int dep_count;
    int wants_count;
    char **wants;
    int conflicts_count;
    char **conflicts;
    int restart_attempts;
    time_t start_time;
    time_t stop_time;
    pthread_mutex_t lock;
    int notification_fd;
    bool enabled;
    bool critical;
    int exit_status;
    char *working_directory;
    char **environment;
    int env_count;
    int restart_delay;
    int watchdog_usec;
    int timeout_start_usec;
    int timeout_stop_usec;
} service_extra_t;

typedef enum {
    EVENT_NONE = 0,
    EVENT_START,
    EVENT_STOP,
    EVENT_RESTART,
    EVENT_RELOAD,
    EVENT_STATUS_CHANGE
} service_event_t;

static service_extra_t service_extras[MAX_SERVICES];
static int epoll_fd;
static pthread_t event_thread;
static volatile bool running = true;
static int socket_fd;

static void *event_loop(void *arg) {
    struct epoll_event events[MAX_EVENTS];
    
    while (running) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            service_event_t event = events[i].events;
            int service_idx = events[i].data.u32;
            
            pthread_mutex_lock(&service_extras[service_idx].lock);
            switch (event) {
                case EVENT_START:
                    if (service_extras[service_idx].enabled) {
                        start_service_with_deps(services[service_idx].name);
                    }
                    break;
                case EVENT_STOP:
                    stop_service_with_deps(services[service_idx].name);
                    break;
                case EVENT_RESTART:
                    restart_service_with_deps(services[service_idx].name);
                    break;
                case EVENT_STATUS_CHANGE:
                    handle_status_change(service_idx);
                    break;
            }
            pthread_mutex_unlock(&service_extras[service_idx].lock);
        }
    }
    return NULL;
}

static int init_socket() {
    socket_fd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (socket_fd == -1) return -1;

    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
    };
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);
    if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(socket_fd);
        return -1;
    }

    if (listen(socket_fd, SOMAXCONN) == -1) {
        close(socket_fd);
        return -1;
    }

    return 0;
}

static int start_service_with_deps(const char *service_name) {
    int service_idx = find_service_idx(service_name);
    if (service_idx == -1) return -1;

    service_extra_t *extra = &service_extras[service_idx];
    
    for (int i = 0; i < extra->dep_count; i++) {
        if (start_service_with_deps(extra->deps[i]) != 0) {
            return -1;
        }
    }

    if (services[service_idx].status == SERVICE_RUNNING) {
        return 0;
    }

    pid_t pid = fork();
    if (pid == 0) {
        if (extra->working_directory) {
            chdir(extra->working_directory);
        }

        for (int i = 0; i < extra->env_count; i++) {
            putenv(extra->environment[i]);
        }

        int null_fd = open("/dev/null", O_RDWR);
        dup2(null_fd, STDIN_FILENO);
        dup2(null_fd, STDOUT_FILENO);
        dup2(null_fd, STDERR_FILENO);
        close(null_fd);

        setsid();

        execlp(service_name, service_name, NULL);
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        services[service_idx].pid = pid;
        services[service_idx].status = SERVICE_RUNNING;
        extra->start_time = time(NULL);
        
        struct epoll_event ev = {
            .events = EPOLLIN | EPOLLOUT,
            .data.u32 = service_idx
        };
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, extra->notification_fd, &ev);
        
        return 0;
    }
    return -1;
}

static int stop_service_with_deps(const char *service_name) {
    int service_idx = find_service_idx(service_name);
    if (service_idx == -1) return -1;

    service_extra_t *extra = &service_extras[service_idx];

    for (int i = 0; i < service_count; i++) {
        service_extra_t *dep_extra = &service_extras[i];
        for (int j = 0; j < dep_extra->dep_count; j++) {
            if (strcmp(dep_extra->deps[j], service_name) == 0) {
                stop_service_with_deps(services[i].name);
            }
        }
    }

    if (services[service_idx].status == SERVICE_STOPPED) {
        return 0;
    }

    if (kill(services[service_idx].pid, SIGTERM) == 0) {
        alarm(extra->timeout_stop_usec / 1000000);
        int status;
        waitpid(services[service_idx].pid, &status, 0);
        alarm(0);
        
        services[service_idx].status = SERVICE_STOPPED;
        services[service_idx].exit_code = status;
        extra->stop_time = time(NULL);
        
        struct epoll_event ev;
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, extra->notification_fd, &ev);
        
        return 0;
    }
    return -1;
}

static void handle_status_change(int service_idx) {
    service_extra_t *extra = &service_extras[service_idx];
    
    if (services[service_idx].status == SERVICE_FAILED && extra->critical) {
        LOG_ERROR("Critical service %s failed, initiating shutdown", 
                  services[service_idx].name);
        emergency_shutdown();
    }

    if (services[service_idx].status == SERVICE_FAILED && 
        extra->restart_attempts < 3) {
        extra->restart_attempts++;
        sleep(extra->restart_delay);
        start_service_with_deps(services[service_idx].name);
    }
}

void initialize_system(void) {
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        LOG_ERROR("Failed to create epoll instance");
        exit(EXIT_FAILURE);
    }

    if (init_socket() == -1) {
        LOG_ERROR("Failed to initialize control socket");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_SERVICES; i++) {
        pthread_mutex_init(&service_extras[i].lock, NULL);
        service_extras[i].notification_fd = eventfd(0, EFD_NONBLOCK);
        service_extras[i].enabled = true;
        service_extras[i].restart_delay = 1;
        service_extras[i].timeout_start_usec = 90000000;
        service_extras[i].timeout_stop_usec = 90000000;
    }

    pthread_create(&event_thread, NULL, event_loop, NULL);
}

void emergency_shutdown(void) {
    running = false;
    for (int i = service_count - 1; i >= 0; i--) {
        if (services[i].status == SERVICE_RUNNING) {
            stop_service_with_deps(services[i].name);
        }
    }
    close(socket_fd);
    unlink(SOCKET_PATH);
    exit(EXIT_FAILURE);
}