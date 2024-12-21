/**
 * @file core.h
 * @brief Core functionality and definitions for Neoinit
 * @author AnmiTaliDev
 * @date 2024-12-21 08:06:43 UTC
 * @version 1.0.0-dev
 * 
 * @copyright Copyright (c) 2024 Nuros Linux. Licensed under GPL-3.0.
 */

#ifndef NEOINIT_CORE_H
#define NEOINIT_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <linux/capability.h>
#include <linux/securebits.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <signal.h>
#include <sched.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <linux/limits.h>

/**
 * @brief Version and build information
 */
#define NEOINIT_VERSION_MAJOR      1
#define NEOINIT_VERSION_MINOR      0
#define NEOINIT_VERSION_PATCH      0
#define NEOINIT_VERSION_SUFFIX    "dev"
#define NEOINIT_VERSION_STRING    "1.0.0-dev"
#define NEOINIT_VERSION_HEX       0x010000
#define NEOINIT_BUILD_DATE        "2024-12-21"
#define NEOINIT_BUILD_TIME        "08:06:43"
#define NEOINIT_BUILD_TIMESTAMP   1703145603ULL
#define NEOINIT_COMPILER_VERSION  __VERSION__
#define NEOINIT_TARGET_ARCH       __architecture__
#define NEOINIT_TARGET_OS         "linux"

/**
 * @brief System paths and configuration
 */
#define NEOINIT_PREFIX            "/usr"
#define NEOINIT_EXEC_PREFIX      NEOINIT_PREFIX
#define NEOINIT_CONF_DIR         "/etc/neoinit"
#define NEOINIT_SERVICES_DIR     NEOINIT_CONF_DIR "/services"
#define NEOINIT_TARGETS_DIR      NEOINIT_CONF_DIR "/targets"
#define NEOINIT_SOCKETS_DIR      NEOINIT_CONF_DIR "/sockets"
#define NEOINIT_TIMERS_DIR       NEOINIT_CONF_DIR "/timers"
#define NEOINIT_RUN_DIR         "/run/neoinit"
#define NEOINIT_CACHE_DIR       "/var/cache/neoinit"
#define NEOINIT_LOG_DIR         "/var/log/neoinit"
#define NEOINIT_LIB_DIR         "/var/lib/neoinit"
#define NEOINIT_CONTROL_SOCKET  NEOINIT_RUN_DIR "/control.sock"
#define NEOINIT_PID_FILE        NEOINIT_RUN_DIR "/neoinit.pid"
#define NEOINIT_STATE_FILE      NEOINIT_RUN_DIR "/state.dat"

/**
 * @brief Critical system limits
 */
enum {
    NEOINIT_MAX_NAME_LEN = 256,
    NEOINIT_MAX_PATH_LEN = PATH_MAX,
    NEOINIT_MAX_CMD_LEN = 4096,
    NEOINIT_MAX_LINE_LEN = 1024,
    NEOINIT_MAX_ARGS = 128,
    NEOINIT_MAX_ENV = 128,
    NEOINIT_MAX_DEPS = 64,
    NEOINIT_MAX_FDS = 1024,
    NEOINIT_MAX_RETRIES = 10,
    NEOINIT_MAX_SERVICES = 1024,
    NEOINIT_MAX_SOCKETS = 64,
    NEOINIT_MAX_TIMERS = 64,
    NEOINIT_MAX_TARGETS = 32,
    NEOINIT_MAX_MOUNTS = 256,
    NEOINIT_MAX_CAPABILITIES = 64,
    NEOINIT_MAX_WATCHERS = 128,
    NEOINIT_MAX_EVENTS = 32,
    NEOINIT_MAX_THREADS = 16,
    NEOINIT_MAX_MESSAGES = 1024,
    NEOINIT_MAX_LOG_SIZE = (10 * 1024 * 1024), // 10MB
    NEOINIT_MAX_STATE_SIZE = (1 * 1024 * 1024), // 1MB
};

/**
 * @brief Return codes for public API
 */
typedef enum {
    NEOINIT_OK = 0,
    NEOINIT_ERROR = -1,
    NEOINIT_ERROR_INVALID_ARG = -2,
    NEOINIT_ERROR_NO_MEMORY = -3,
    NEOINIT_ERROR_IO = -4,
    NEOINIT_ERROR_PERMISSION = -5,
    NEOINIT_ERROR_NOT_FOUND = -6,
    NEOINIT_ERROR_EXISTS = -7,
    NEOINIT_ERROR_BUSY = -8,
    NEOINIT_ERROR_TIMEOUT = -9,
    NEOINIT_ERROR_AGAIN = -10,
    NEOINIT_ERROR_INTERRUPTED = -11,
    NEOINIT_ERROR_NOT_SUPPORTED = -12,
    NEOINIT_ERROR_DEPENDENCY = -13,
    NEOINIT_ERROR_RESOURCE = -14,
    NEOINIT_ERROR_PROTOCOL = -15,
    NEOINIT_ERROR_STATE = -16,
    NEOINIT_ERROR_INIT = -17,
    NEOINIT_ERROR_SHUTDOWN = -18,
    NEOINIT_ERROR_SIGNAL = -19,
    NEOINIT_ERROR_SYSTEM = -20,
} neoinit_error_t;

/**
 * @brief Log levels
 */
typedef enum {
    NEOINIT_LOG_EMERG = 0,   // System is unusable
    NEOINIT_LOG_ALERT,       // Action must be taken immediately
    NEOINIT_LOG_CRIT,        // Critical conditions
    NEOINIT_LOG_ERROR,       // Error conditions
    NEOINIT_LOG_WARNING,     // Warning conditions
    NEOINIT_LOG_NOTICE,      // Normal but significant condition
    NEOINIT_LOG_INFO,        // Informational
    NEOINIT_LOG_DEBUG,       // Debug-level messages
    NEOINIT_LOG_TRACE,       // Trace-level messages
} neoinit_log_level_t;

/**
 * @brief Service states with detailed status information
 */
typedef enum {
    NEOINIT_SERVICE_INACTIVE = 0,     // Not running
    NEOINIT_SERVICE_STARTING,         // Starting up
    NEOINIT_SERVICE_RUNNING,          // Running normally
    NEOINIT_SERVICE_RELOADING,        // Reloading configuration
    NEOINIT_SERVICE_STOPPING,         // Shutting down
    NEOINIT_SERVICE_STOPPED,          // Stopped normally
    NEOINIT_SERVICE_FAILED,           // Failed to start/run
    NEOINIT_SERVICE_RESTARTING,       // Restarting
    NEOINIT_SERVICE_MAINTENANCE,      // In maintenance mode
    NEOINIT_SERVICE_DEGRADED,         // Running but degraded
    NEOINIT_SERVICE_RESERVED1,        // Reserved for future use
    NEOINIT_SERVICE_RESERVED2,        // Reserved for future use
    NEOINIT_SERVICE_STATE_MAX
} neoinit_service_state_t;

/**
 * @brief Service types
 */
typedef enum {
    NEOINIT_SERVICE_TYPE_SIMPLE = 0,  // Simple foreground service
    NEOINIT_SERVICE_TYPE_FORKING,     // Traditional forking daemon
    NEOINIT_SERVICE_TYPE_ONESHOT,     // One-time execution
    NEOINIT_SERVICE_TYPE_NOTIFY,      // Service notifies readiness
    NEOINIT_SERVICE_TYPE_DBUS,        // D-Bus activated service
    NEOINIT_SERVICE_TYPE_IDLE,        // Run when system is idle
    NEOINIT_SERVICE_TYPE_SOCKET,      // Socket activated service
    NEOINIT_SERVICE_TYPE_TIMER,       // Timer activated service
    NEOINIT_SERVICE_TYPE_TARGET,      // Grouping target
    NEOINIT_SERVICE_TYPE_PATH,        // Path monitored service
    NEOINIT_SERVICE_TYPE_MOUNT,       // Mount point
    NEOINIT_SERVICE_TYPE_CONTAINER,   // Container service
    NEOINIT_SERVICE_TYPE_RESERVED1,   // Reserved for future use
    NEOINIT_SERVICE_TYPE_RESERVED2,   // Reserved for future use
    NEOINIT_SERVICE_TYPE_MAX
} neoinit_service_type_t;

/**
 * @brief Service flags
 */
typedef enum {
    NEOINIT_FLAG_NONE = 0,
    NEOINIT_FLAG_CRITICAL = (1 << 0),        // Critical system service
    NEOINIT_FLAG_ESSENTIAL = (1 << 1),       // Essential for boot
    NEOINIT_FLAG_ISOLATE = (1 << 2),         // Stop others when starting
    NEOINIT_FLAG_IGNORE_DEPS = (1 << 3),     // Ignore dependencies
    NEOINIT_FLAG_NO_BLOCK = (1 << 4),        // Don't block startup
    NEOINIT_FLAG_RELOAD = (1 << 5),          // Can be reloaded
    NEOINIT_FLAG_KILL_MODE = (1 << 6),       // Kill all processes
    NEOINIT_FLAG_NO_RESPAWN = (1 << 7),      // Don't respawn
    NEOINIT_FLAG_ACCEPT = (1 << 8),          // Accept socket connections
    NEOINIT_FLAG_NOTIFY = (1 << 9),          // Uses notification
    NEOINIT_FLAG_REMAIN = (1 << 10),         // Remain after exit
    NEOINIT_FLAG_OVERRIDE = (1 << 11),       // Override file exists
    NEOINIT_FLAG_DYNAMIC = (1 << 12),        // Dynamically loaded
    NEOINIT_FLAG_EARLY = (1 << 13),          // Early boot service
    NEOINIT_FLAG_LATE = (1 << 14),           // Late boot service
    NEOINIT_FLAG_USER = (1 << 15),           // User service
} neoinit_service_flags_t;

/**
 * @brief Service dependency types
 */
typedef enum {
    NEOINIT_DEP_NONE = 0,
    NEOINIT_DEP_REQUIRES = (1 << 0),      // Hard dependency
    NEOINIT_DEP_WANTS = (1 << 1),         // Soft dependency
    NEOINIT_DEP_REQUISITE = (1 << 2),     // Must exist
    NEOINIT_DEP_CONFLICTS = (1 << 3),     // Cannot coexist
    NEOINIT_DEP_BEFORE = (1 << 4),        // Order before
    NEOINIT_DEP_AFTER = (1 << 5),         // Order after
    NEOINIT_DEP_BINDS_TO = (1 << 6),      // Bound lifetime
    NEOINIT_DEP_PART_OF = (1 << 7),       // Part of unit
    NEOINIT_DEP_UPHOLDS = (1 << 8),       // Keep running
    NEOINIT_DEP_REQUIRED_BY = (1 << 9),   // Reverse requires
    NEOINIT_DEP_WANTED_BY = (1 << 10),    // Reverse wants
    NEOINIT_DEP_BOUND_BY = (1 << 11),     // Reverse binds
}

/**
 * @brief Service runtime statistics
 */
typedef struct {
    // CPU statistics
    uint64_t cpu_usage;             // Total CPU time in microseconds
    uint64_t cpu_user_time;         // User CPU time
    uint64_t cpu_system_time;       // System CPU time
    float cpu_percentage;           // Current CPU usage percentage
    uint32_t cpu_throttled_count;   // Number of times CPU was throttled
    uint64_t cpu_throttled_time;    // Total time CPU was throttled

    // Memory statistics
    uint64_t memory_current;        // Current memory usage
    uint64_t memory_peak;           // Peak memory usage
    uint64_t memory_swap_current;   // Current swap usage
    uint64_t memory_swap_peak;      // Peak swap usage
    uint64_t memory_fault_count;    // Page fault count
    uint64_t memory_mapped;         // Mapped memory size

    // I/O statistics
    uint64_t io_read_bytes;         // Total bytes read
    uint64_t io_write_bytes;        // Total bytes written
    uint64_t io_read_ops;          // Total read operations
    uint64_t io_write_ops;         // Total write operations
    uint64_t io_queued;            // Currently queued I/O operations
    
    // Network statistics (if applicable)
    uint64_t net_rx_bytes;         // Total bytes received
    uint64_t net_tx_bytes;         // Total bytes transmitted
    uint64_t net_rx_packets;       // Total packets received
    uint64_t net_tx_packets;       // Total packets transmitted
    uint64_t net_errors;           // Network errors count

    // Process statistics
    pid_t pid;                     // Current process ID
    pid_t ppid;                    // Parent process ID
    uint32_t threads_count;        // Number of threads
    uint32_t fd_count;             // Number of open file descriptors
    uint32_t socket_count;         // Number of open sockets

    // Timing information
    time_t start_time;             // Last start time
    time_t stop_time;              // Last stop time
    uint64_t uptime;               // Total uptime in seconds
    uint64_t downtime;             // Total downtime in seconds

    // Restart statistics
    uint32_t restart_count;        // Total number of restarts
    uint32_t failure_count;        // Number of failures
    time_t last_restart_time;      // Time of last restart
    int last_exit_code;           // Last exit code

    // Resource limit hits
    uint32_t memory_limit_hits;    // Times memory limit was hit
    uint32_t cpu_limit_hits;       // Times CPU limit was hit
    uint32_t file_limit_hits;      // Times file limit was hit
} neoinit_service_stats_t;

/**
 * @brief Complete service runtime state
 */
typedef struct {
    // Basic identification
    char name[NEOINIT_MAX_NAME_LENGTH];
    uint64_t service_id;           // Unique service identifier

    // Current state
    neoinit_service_state_t state;
    uint32_t state_flags;          // Additional state flags
    char state_message[NEOINIT_MAX_CMD_LENGTH];  // Current state message

    // Configuration
    neoinit_service_config_t config;
    bool config_valid;             // Is configuration valid?
    time_t config_mtime;          // Last config modification time

    // Process management
    pid_t main_pid;               // Main process ID
    pid_t control_pid;            // Control process ID (if any)
    pid_t *child_pids;            // Array of child PIDs
    size_t child_count;           // Number of child processes

    // Statistics
    neoinit_service_stats_t stats;
    neoinit_service_stats_t stats_previous;  // Previous stats for delta calculation

    // Socket activation
    int *socket_fds;              // Array of socket FDs
    size_t socket_fd_count;       // Number of socket FDs
    
    // Resource tracking
    struct {
        int64_t oom_score;        // OOM score
        uint64_t memory_high;     // High memory watermark
        uint64_t memory_max;      // Maximum memory limit
        char cgroup_path[NEOINIT_MAX_PATH_LENGTH];  // CGroup path
    } resources;

    // Dependency management
    struct {
        char **dep_names;         // Array of dependency names
        size_t dep_count;         // Number of dependencies
        bool deps_satisfied;      // Are all dependencies satisfied?
        uint32_t dep_timeout;     // Dependency timeout counter
    } deps;

    // Security context
    struct {
        uid_t run_as_uid;        // Running UID
        gid_t run_as_gid;        // Running GID
        mode_t umask;            // Current umask
        char *selinux_context;   // SELinux context
        char *apparmor_profile;  // AppArmor profile
    } security;

    // Namespace information
    struct {
        int mnt_ns;              // Mount namespace FD
        int net_ns;              // Network namespace FD
        int pid_ns;              // PID namespace FD
        int ipc_ns;              // IPC namespace FD
        int user_ns;             // User namespace FD
        bool namespaced;         // Is service running in namespace?
    } namespaces;

    // Runtime directories
    struct {
        char runtime_dir[NEOINIT_MAX_PATH_LENGTH];
        char state_dir[NEOINIT_MAX_PATH_LENGTH];
        char cache_dir[NEOINIT_MAX_PATH_LENGTH];
        char logs_dir[NEOINIT_MAX_PATH_LENGTH];
    } dirs;

    // Watchdog
    struct {
        bool enabled;
        uint32_t timeout;
        time_t last_ping;
        uint32_t missed_pings;
    } watchdog;

    // Notification
    struct {
        int notify_fd;           // Notification socket
        uint32_t notify_state;   // Current notification state
        char notify_msg[NEOINIT_MAX_CMD_LENGTH];
    } notify;

    // Throttling
    struct {
        time_t throttle_until;   // Throttle until this time
        uint32_t throttle_count; // Number of throttle events
        uint32_t backoff_delay;  // Current backoff delay
    } throttle;

} neoinit_service_t;

/**
 * @brief Core system functions
 */

// Initialization and shutdown
int neoinit_core_init(void);
int neoinit_core_shutdown(void);
int neoinit_core_reload(void);
int neoinit_core_emergency(void);

// Service management
int neoinit_service_create(const char *name, neoinit_service_t **service);
int neoinit_service_destroy(neoinit_service_t *service);
int neoinit_service_start(neoinit_service_t *service);
int neoinit_service_stop(neoinit_service_t *service);
int neoinit_service_reload(neoinit_service_t *service);
int neoinit_service_restart(neoinit_service_t *service);

// Configuration management
int neoinit_config_load(const char *path, neoinit_service_config_t *config);
int neoinit_config_validate(const neoinit_service_config_t *config);
int neoinit_config_apply(neoinit_service_t *service, const neoinit_service_config_t *config);
int neoinit_config_save(const char *path, const neoinit_service_config_t *config);

// Resource management
int neoinit_resources_apply(neoinit_service_t *service);
int neoinit_resources_update(neoinit_service_t *service);
int neoinit_resources_reset(neoinit_service_t *service);

// Dependency management
int neoinit_deps_check(neoinit_service_t *service);
int neoinit_deps_resolve(neoinit_service_t *service);
int neoinit_deps_order(neoinit_service_t **services, size_t count);

// State management
neoinit_service_state_t neoinit_state_get(const neoinit_service_t *service);
const char *neoinit_state_to_string(neoinit_service_state_t state);
int neoinit_state_wait(neoinit_service_t *service, neoinit_service_state_t state, uint32_t timeout);

// Statistics and monitoring
int neoinit_stats_update(neoinit_service_t *service);
int neoinit_stats_reset(neoinit_service_t *service);
int neoinit_stats_get_json(const neoinit_service_t *service, char *buf, size_t size);

// Security management
int neoinit_security_apply(neoinit_service_t *service);
int neoinit_security_check(const neoinit_service_t *service);
int neoinit_capabilities_drop(neoinit_service_t *service);

// Namespace management
int neoinit_namespace_setup(neoinit_service_t *service);
int neoinit_namespace_enter(neoinit_service_t *service);
int neoinit_namespace_cleanup(neoinit_service_t *service);

// Logging and debugging
void neoinit_log(int level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
void neoinit_debug(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
int neoinit_dump_state(const neoinit_service_t *service, int fd);

// Error handling
const char *neoinit_error_string(neoinit_error_t error);
void neoinit_error_set(neoinit_error_t error, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
neoinit_error_t neoinit_error_get(void);

// Utility functions
uint64_t neoinit_get_monotonic_time(void);
uint64_t neoinit_get_boottime(void);
int neoinit_generate_unit_name(char *buf, size_t size, const char *fmt, ...);

#endif /* NEOINIT_CORE_H */