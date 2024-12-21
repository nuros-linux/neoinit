/**
 * @file events.h
 * @brief Event handling and notification system for Neoinit
 * @author AnmiTaliDev
 * @date 2024-12-21 08:19:23 UTC
 * @version 1.0.0
 * 
 * @copyright Copyright (c) 2024 Nuros Linux. Licensed under GPL-3.0.
 * 
 * This header defines the event system for Neoinit, providing a comprehensive
 * event handling infrastructure for service management and system monitoring.
 */

#ifndef NEOINIT_EVENTS_H
#define NEOINIT_EVENTS_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include <sys/inotify.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include "neoinit/core.h"

/**
 * @brief Maximum event queue size
 */
#define NEOINIT_MAX_EVENTS 1024
#define NEOINIT_MAX_EVENT_HANDLERS 64
#define NEOINIT_MAX_EVENT_SOURCES 256
#define NEOINIT_MAX_EVENT_NAME_LENGTH 128
#define NEOINIT_MAX_EVENT_DATA_SIZE 4096

/**
 * @brief Event priorities
 */
typedef enum {
    NEOINIT_EVENT_PRIORITY_EMERGENCY = 0,  // System is unusable
    NEOINIT_EVENT_PRIORITY_ALERT,          // Action must be taken immediately
    NEOINIT_EVENT_PRIORITY_CRITICAL,       // Critical conditions
    NEOINIT_EVENT_PRIORITY_ERROR,          // Error conditions
    NEOINIT_EVENT_PRIORITY_WARNING,        // Warning conditions
    NEOINIT_EVENT_PRIORITY_NOTICE,         // Normal but significant condition
    NEOINIT_EVENT_PRIORITY_INFO,           // Informational
    NEOINIT_EVENT_PRIORITY_DEBUG           // Debug-level messages
} neoinit_event_priority_t;

/**
 * @brief Event types
 */
typedef enum {
    // System events
    NEOINIT_EVENT_SYSTEM_STARTUP = 0,
    NEOINIT_EVENT_SYSTEM_SHUTDOWN,
    NEOINIT_EVENT_SYSTEM_RELOAD,
    NEOINIT_EVENT_SYSTEM_ERROR,
    NEOINIT_EVENT_SYSTEM_OOM,
    NEOINIT_EVENT_SYSTEM_CRASH,

    // Service events
    NEOINIT_EVENT_SERVICE_START,
    NEOINIT_EVENT_SERVICE_STOP,
    NEOINIT_EVENT_SERVICE_RELOAD,
    NEOINIT_EVENT_SERVICE_RESTART,
    NEOINIT_EVENT_SERVICE_FAIL,
    NEOINIT_EVENT_SERVICE_EXIT,
    NEOINIT_EVENT_SERVICE_WATCHDOG,
    NEOINIT_EVENT_SERVICE_THROTTLED,
    NEOINIT_EVENT_SERVICE_OOM,

    // Resource events
    NEOINIT_EVENT_RESOURCE_LOW_MEMORY,
    NEOINIT_EVENT_RESOURCE_HIGH_CPU,
    NEOINIT_EVENT_RESOURCE_HIGH_IO,
    NEOINIT_EVENT_RESOURCE_LIMIT_HIT,
    
    // Network events
    NEOINIT_EVENT_NETWORK_UP,
    NEOINIT_EVENT_NETWORK_DOWN,
    NEOINIT_EVENT_NETWORK_ERROR,
    
    // Security events
    NEOINIT_EVENT_SECURITY_VIOLATION,
    NEOINIT_EVENT_SECURITY_BREACH,
    NEOINIT_EVENT_SECURITY_SELINUX,
    NEOINIT_EVENT_SECURITY_APPARMOR,
    
    // Configuration events
    NEOINIT_EVENT_CONFIG_CHANGED,
    NEOINIT_EVENT_CONFIG_ERROR,
    NEOINIT_EVENT_CONFIG_RELOAD,
    
    // Dependency events
    NEOINIT_EVENT_DEPENDENCY_SATISFIED,
    NEOINIT_EVENT_DEPENDENCY_FAILED,
    NEOINIT_EVENT_DEPENDENCY_TIMEOUT,
    
    // Timer events
    NEOINIT_EVENT_TIMER_TRIGGERED,
    NEOINIT_EVENT_TIMER_EXPIRED,
    NEOINIT_EVENT_TIMER_ERROR,
    
    // Socket events
    NEOINIT_EVENT_SOCKET_ACTIVATED,
    NEOINIT_EVENT_SOCKET_CLOSED,
    NEOINIT_EVENT_SOCKET_ERROR,
    
    // File system events
    NEOINIT_EVENT_FS_FULL,
    NEOINIT_EVENT_FS_ERROR,
    NEOINIT_EVENT_FS_READONLY,
    
    // Hardware events
    NEOINIT_EVENT_HW_ERROR,
    NEOINIT_EVENT_HW_ADDED,
    NEOINIT_EVENT_HW_REMOVED,
    
    // Custom events
    NEOINIT_EVENT_CUSTOM_BASE = 1000
} neoinit_event_type_t;

/**
 * @brief Event flags
 */
typedef enum {
    NEOINIT_EVENT_FLAG_NONE        = 0,
    NEOINIT_EVENT_FLAG_PERSISTENT  = 1 << 0,  // Event persists after handling
    NEOINIT_EVENT_FLAG_SYNC        = 1 << 1,  // Synchronous handling required
    NEOINIT_EVENT_FLAG_BROADCAST   = 1 << 2,  // Broadcast to all handlers
    NEOINIT_EVENT_FLAG_QUEUED      = 1 << 3,  // Event is queued
    NEOINIT_EVENT_FLAG_INTERNAL    = 1 << 4,  // Internal event
    NEOINIT_EVENT_FLAG_EXTERNAL    = 1 << 5,  // External event
    NEOINIT_EVENT_FLAG_LOGGED      = 1 << 6,  // Event should be logged
    NEOINIT_EVENT_FLAG_CRITICAL    = 1 << 7,  // Critical event
    NEOINIT_EVENT_FLAG_THROTTLED   = 1 << 8,  // Event is throttled
    NEOINIT_EVENT_FLAG_FILTERED    = 1 << 9   // Event is filtered
} neoinit_event_flags_t;

/**
 * @brief Event source types
 */
typedef enum {
    NEOINIT_EVENT_SOURCE_INTERNAL,     // Internal event generation
    NEOINIT_EVENT_SOURCE_SIGNAL,       // Signal-based events
    NEOINIT_EVENT_SOURCE_TIMER,        // Timer-based events
    NEOINIT_EVENT_SOURCE_INOTIFY,      // File system events
    NEOINIT_EVENT_SOURCE_NETLINK,      // Network events
    NEOINIT_EVENT_SOURCE_SOCKET,       // Socket events
    NEOINIT_EVENT_SOURCE_DBUS,         // D-Bus events
    NEOINIT_EVENT_SOURCE_CUSTOM        // Custom event sources
} neoinit_event_source_type_t;

/**
 * @brief Event data structure
 */
typedef struct {
    uint64_t id;                       // Unique event ID
    neoinit_event_type_t type;         // Event type
    neoinit_event_priority_t priority; // Event priority
    neoinit_event_flags_t flags;       // Event flags
    time_t timestamp;                  // Event timestamp
    char name[NEOINIT_MAX_EVENT_NAME_LENGTH];  // Event name
    
    // Source information
    struct {
        neoinit_event_source_type_t type;  // Source type
        char name[NEOINIT_MAX_EVENT_NAME_LENGTH];  // Source name
        uint64_t id;                   // Source ID
    } source;
    
    // Target information
    struct {
        char service[NEOINIT_MAX_NAME_LENGTH];  // Target service name
        pid_t pid;                     // Target process ID
        uint64_t id;                   // Target ID
    } target;
    
    // Event data
    struct {
        void *ptr;                     // Generic data pointer
        size_t size;                   // Data size
        char buf[NEOINIT_MAX_EVENT_DATA_SIZE];  // Data buffer
    } data;
    
    // Event context
    struct {
        uint64_t sequence;             // Sequence number
        uint32_t generation;           // Generation count
        void *user_data;               // User data pointer
    } context;
} neoinit_event_t;

/**
 * @brief Event handler callback function type
 */
typedef int (*neoinit_event_handler_fn)(const neoinit_event_t *event, void *user_data);

/**
 * @brief Event handler configuration
 */
typedef struct {
    neoinit_event_handler_fn handler;  // Handler function
    void *user_data;                   // User data for handler
    neoinit_event_type_t type;         // Event type to handle
    neoinit_event_priority_t priority; // Handler priority
    uint32_t flags;                    // Handler flags
    char name[NEOINIT_MAX_EVENT_NAME_LENGTH];  // Handler name
} neoinit_event_handler_config_t;

/**
 * @brief Event source configuration
 */
typedef struct {
    neoinit_event_source_type_t type;  // Source type
    char name[NEOINIT_MAX_EVENT_NAME_LENGTH];  // Source name
    uint32_t flags;                    // Source flags
    void *user_data;                   // User data for source
    
    union {
        // Signal source configuration
        struct {
            sigset_t mask;             // Signal mask
            int signo;                 // Signal number
        } signal;
        
        // Timer source configuration
        struct {
            uint64_t interval;         // Timer interval
            uint64_t initial;          // Initial delay
            bool absolute;             // Absolute time
        } timer;
        
        // Inotify source configuration
        struct {
            char path[NEOINIT_MAX_PATH_LENGTH];  // Watch path
            uint32_t mask;             // Watch mask
        } inotify;
        
        // Socket source configuration
        struct {
            int type;                  // Socket type
            char path[NEOINIT_MAX_PATH_LENGTH];  // Socket path
        } socket;
    } config;
} neoinit_event_source_config_t;

/**
 * @brief Event system functions
 */

// Initialization and cleanup
int neoinit_events_init(void);
int neoinit_events_cleanup(void);

// Event handling
int neoinit_event_emit(neoinit_event_t *event);
int neoinit_event_broadcast(neoinit_event_t *event);
int neoinit_event_cancel(uint64_t event_id);

// Handler management
int neoinit_handler_register(const neoinit_event_handler_config_t *config);
int neoinit_handler_unregister(const char *name);
int neoinit_handler_enable(const char *name);
int neoinit_handler_disable(const char *name);

// Source management
int neoinit_source_register(const neoinit_event_source_config_t *config);
int neoinit_source_unregister(const char *name);
int neoinit_source_enable(const char *name);
int neoinit_source_disable(const char *name);

// Event loop control
int neoinit_events_dispatch(void);
int neoinit_events_dispatch_once(int timeout_ms);
int neoinit_events_stop(void);

// Event queue management
int neoinit_queue_flush(void);
size_t neoinit_queue_size(void);
bool neoinit_queue_is_empty(void);

// Event filtering
int neoinit_filter_add(neoinit_event_type_t type, const char *pattern);
int neoinit_filter_remove(neoinit_event_type_t type, const char *pattern);
int neoinit_filter_clear(void);

// Utility functions
const char *neoinit_event_type_to_string(neoinit_event_type_t type);
const char *neoinit_priority_to_string(neoinit_event_priority_t priority);
uint64_t neoinit_event_get_current_id(void);

// Debugging and monitoring
int neoinit_events_dump_state(int fd);
int neoinit_events_get_stats(char *buf, size_t size);
void neoinit_events_debug_enable(void);
void neoinit_events_debug_disable(void);

#endif /* NEOINIT