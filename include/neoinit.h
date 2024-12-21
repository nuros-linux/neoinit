#ifndef NEOINIT_H
#define NEOINIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <sys/resource.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <linux/limits.h>

#define MAX_SERVICE_NAME_LENGTH 128
#define MAX_SERVICES 1024
#define MAX_DEPS 32
#define MAX_ENV_VARS 128
#define MAX_EVENTS 64
#define MAX_SOCKET_CONNECTIONS 128
#define SOCKET_PATH "/run/neoinit.sock"
#define CONFIG_PATH "/etc/neoinit"
#define SERVICE_PATH "/etc/neoinit/services"
#define LOG_PATH "/var/log/neoinit"
#define DEFAULT_TIMEOUT_USEC 90000000
#define WATCHDOG_INTERVAL_USEC 30000000

typedef enum {
    SERVICE_STOPPED,
    SERVICE_STARTING,
    SERVICE_RUNNING,
    SERVICE_RELOADING,
    SERVICE_STOPPING,
    SERVICE_RESTARTING,
    SERVICE_FAILED,
    SERVICE_MAINTENANCE
} service_status_t;

typedef enum {
    EVENT_NONE,
    EVENT_START,
    EVENT_STOP,
    EVENT_RESTART,
    EVENT_RELOAD,
    EVENT_STATUS_CHANGE,
    EVENT_DEPENDENCY_FAILURE,
    EVENT_RESOURCE_EXHAUSTED,
    EVENT_WATCHDOG_TRIGGER,
    EVENT_SOCKET_ACTIVATION
} service_event_t;

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL
} log_level_t;

typedef enum {
    RESTART_NEVER,
    RESTART_ON_FAILURE,
    RESTART_ALWAYS,
    RESTART_ON_WATCHDOG,
    RESTART_ON_ABNORMAL
} restart_policy_t;

typedef struct {
    int cpu_weight;
    int io_weight;
    rlim_t memory_limit;
    rlim_t files_limit;
    rlim_t tasks_limit;
    char *cpu_affinity;
    bool realtime;
    int rt_priority;
    int nice_level;
} resource_limits_t;

typedef struct {
    char *mount_point;
    char *mount_options;
    bool required;
    bool lazy_unmount;
} mount_requirements_t;

typedef struct {
    char name[MAX_SERVICE_NAME_LENGTH];
    service_status_t status;
    pid_t pid;
    int exit_code;
    time_t start_time;
    time_t stop_time;
    
    char **dependencies;
    int dep_count;
    char **wants;
    int wants_count;
    char **conflicts;
    int conflicts_count;
    
    char *working_directory;
    char **environment;
    int env_count;
    
    int restart_attempts;
    restart_policy_t restart_policy;
    int restart_delay;
    
    int notification_fd;
    int watchdog_usec;
    int timeout_start_usec;
    int timeout_stop_usec;
    
    resource_limits_t resources;
    mount_requirements_t *mounts;
    int mount_count;
    
    bool enabled;
    bool critical;
    bool socket_activated;
    bool keep_alive;
    
    pthread_mutex_t lock;
    void *private_data;
} service_t;

typedef struct {
    int epoll_fd;
    int socket_fd;
    pthread_t event_thread;
    volatile bool running;
    service_t services[MAX_SERVICES];
    int service_count;
} neoinit_context_t;

// Core system functions
void initialize_system(void);
void shutdown_system(void);
void emergency_shutdown(void);
int reload_configuration(void);

// Service management
int start_service(const char *service_name);
int stop_service(const char *service_name);
int restart_service(const char *service_name);
int reload_service(const char *service_name);
service_status_t get_service_status(const char *service_name);
void print_all_services_status(void);

// Advanced service management
int start_service_with_deps(const char *service_name);
int stop_service_with_deps(const char *service_name);
int restart_service_with_deps(const char *service_name);
int enable_service(const char *service_name);
int disable_service(const char *service_name);
int mask_service(const char *service_name);
int unmask_service(const char *service_name);

// Resource management
int set_service_resources(const char *service_name, resource_limits_t *limits);
int get_service_resources(const char *service_name, resource_limits_t *limits);
int adjust_service_priority(const char *service_name, int nice_value);
int set_service_cpu_affinity(const char *service_name, const char *cpu_list);

// Dependency management
int add_service_dependency(const char *service_name, const char *dependency);
int remove_service_dependency(const char *service_name, const char *dependency);
int add_service_want(const char *service_name, const char *wanted_service);
int add_service_conflict(const char *service_name, const char *conflicting_service);

// Socket activation
int setup_socket_activation(const char *service_name, const char *socket_path);
int accept_socket_connection(const char *service_name);

// Environment management
int set_service_environment(const char *service_name, const char *var, const char *value);
int clear_service_environment(const char *service_name);
char *get_service_environment(const char *service_name, const char *var);

// Mount management
int add_mount_requirement(const char *service_name, mount_requirements_t *mount);
int remove_mount_requirement(const char *service_name, const char *mount_point);

// Watchdog management
int enable_service_watchdog(const char *service_name, int timeout_usec);
int disable_service_watchdog(const char *service_name);
int ping_watchdog(const char *service_name);

// Logging
void log_message(log_level_t level, const char *format, ...);
int set_log_level(log_level_t level);
int set_log_target(const char *target);
int rotate_logs(void);

// Event handling
void *event_loop(void *arg);
int handle_service_event(service_event_t event, const char *service_name);
int register_event_handler(service_event_t event, void (*handler)(const char *));

// Utility functions
int find_service_idx(const char *service_name);
char *get_service_description(const char *service_name);
int get_service_statistics(const char *service_name, char *stats_buf, size_t buf_size);
int verify_service_integrity(const char *service_name);

// Control interface
int init_control_socket(void);
int process_control_command(const char *command, char *response, size_t response_size);
int broadcast_status_change(const char *service_name, service_status_t new_status);

#ifdef DEBUG
#define LOG_DEBUG(fmt, ...) log_message(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif

#define LOG_INFO(fmt, ...) log_message(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) log_message(LOG_LEVEL_WARNING, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_message(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...) log_message(LOG_LEVEL_CRITICAL, fmt, ##__VA_ARGS__)

#endif // NEOINIT_H