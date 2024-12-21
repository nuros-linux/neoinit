# Neoinit API Documentation

## Table of Contents

1. [Core API](#core-api)
2. [Service Management](#service-management)
3. [Event Handling](#event-handling)
4. [Resource Management](#resource-management)
5. [Socket Activation](#socket-activation)
6. [Logging](#logging)
7. [Error Codes](#error-codes)
8. [Data Structures](#data-structures)
9. [Examples](#examples)

## Core API

### System Initialization

```c
void initialize_system(void);
```
Initializes the Neoinit system. Must be called before any other API functions.

### System Shutdown

```c
void shutdown_system(void);
void emergency_shutdown(void);
```
Gracefully shuts down the system or performs emergency shutdown.

### Configuration

```c
int reload_configuration(void);
```
Reloads system configuration without restart.

**Returns:** 0 on success, -1 on failure

## Service Management

### Basic Service Control

```c
int start_service(const char *service_name);
int stop_service(const char *service_name);
int restart_service(const char *service_name);
int reload_service(const char *service_name);
```

**Parameters:**
- `service_name`: Name of the service to control

**Returns:** 0 on success, -1 on failure

### Advanced Service Control

```c
int start_service_with_deps(const char *service_name);
int stop_service_with_deps(const char *service_name);
int enable_service(const char *service_name);
int disable_service(const char *service_name);
int mask_service(const char *service_name);
int unmask_service(const char *service_name);
```

### Service Status

```c
service_status_t get_service_status(const char *service_name);
void print_all_services_status(void);
```

## Event Handling

### Event Loop

```c
void *event_loop(void *arg);
int handle_service_event(service_event_t event, const char *service_name);
```

### Event Registration

```c
int register_event_handler(service_event_t event, void (*handler)(const char *));
```

## Resource Management

### Resource Limits

```c
int set_service_resources(const char *service_name, resource_limits_t *limits);
int get_service_resources(const char *service_name, resource_limits_t *limits);
```

### CPU Management

```c
int adjust_service_priority(const char *service_name, int nice_value);
int set_service_cpu_affinity(const char *service_name, const char *cpu_list);
```

## Socket Activation

```c
int setup_socket_activation(const char *service_name, const char *socket_path);
int accept_socket_connection(const char *service_name);
```

## Logging

```c
void log_message(log_level_t level, const char *format, ...);
int set_log_level(log_level_t level);
int set_log_target(const char *target);
int rotate_logs(void);
```

## Data Structures

### Service Status

```c
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
```

### Resource Limits

```c
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
```

### Service Structure

```c
typedef struct {
    char name[MAX_SERVICE_NAME_LENGTH];
    service_status_t status;
    pid_t pid;
    int exit_code;
    time_t start_time;
    time_t stop_time;
    char **dependencies;
    int dep_count;
    resource_limits_t resources;
    bool enabled;
    bool critical;
} service_t;
```

## Error Codes

| Code | Description |
|------|-------------|
| 0    | Success |
| -1   | Generic failure |
| -2   | Service not found |
| -3   | Resource limit exceeded |
| -4   | Permission denied |
| -5   | Invalid configuration |
| -6   | Dependency error |

## Examples

### Basic Service Management

```c
// Initialize the system
initialize_system();

// Start a service
if (start_service("nginx") == 0) {
    printf("nginx started successfully\n");
}

// Check service status
service_status_t status = get_service_status("nginx");
if (status == SERVICE_RUNNING) {
    printf("nginx is running\n");
}

// Stop the service
stop_service("nginx");
```

### Resource Management

```c
// Set resource limits
resource_limits_t limits = {
    .cpu_weight = 100,
    .memory_limit = 512 * 1024 * 1024,  // 512MB
    .files_limit = 1000
};

set_service_resources("nginx", &limits);
```

### Event Handling

```c
// Define event handler
void on_service_failure(const char *service_name) {
    log_message(LOG_LEVEL_ERROR, "Service %s failed", service_name);
    restart_service(service_name);
}

// Register event handler
register_event_handler(EVENT_FAILURE, on_service_failure);
```

## Additional Notes

1. All functions are thread-safe unless explicitly stated otherwise.
2. Memory management is handled internally by the library.
3. Service names are case-sensitive.
4. Maximum service name length is defined by MAX_SERVICE_NAME_LENGTH.

## Best Practices

1. Always check return values for error conditions.
2. Use dependency management for complex service relationships.
3. Implement proper error handling in event callbacks.
4. Monitor resource usage for critical services.
5. Use logging appropriately for debugging and monitoring.

---

For more examples and detailed documentation, please refer to the [examples/](../examples/) directory and other documentation in the [docs/](../docs/) folder.

*Note: This API documentation is subject to change in future releases.*