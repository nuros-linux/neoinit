# Neoinit Installation Guide
## Important Notice

⚠️ **Installation Restrictions**

Neoinit can ONLY be installed on:
- Linux From Scratch (LFS) systems
- Independent Linux distributions
- NurOS

**DO NOT** attempt to install Neoinit on other Linux distributions as it may cause system instability or boot failures.

## Table of Contents
1. [System Requirements](#system-requirements)
2. [Building from Source](#building-from-source)
3. [Configuration](#configuration)
4. [Troubleshooting](#troubleshooting)

## System Requirements

### Minimum Requirements
- Linux From Scratch 11.0 or higher
- 64-bit architecture (x86_64, aarch64)
- 64 MB RAM
- 100 MB free disk space

### Build Dependencies
These must be compiled from source on LFS systems:
```bash
# Required tools
- gcc (10.2.0 or higher)
- make (4.3 or higher)
- binutils (2.37 or higher)
- cppcheck (optional)
- valgrind (optional)
- doxygen (optional for documentation)
```

## Building from Source

### 1. Clone Repository
```bash
git clone https://github.com/nuros-linux/neoinit.git
cd neoinit
```

### 2. Build Configuration
```bash
# Create build directory
mkdir build
cd build

# Standard build
make

# Debug build
make debug

# Performance optimized build
make CFLAGS="-O3 -march=native"
```

### 3. Installation
```bash
# Install to system
make install

# Verify installation
neoinit --version
```

## Configuration

### 1. Main Configuration Files
```bash
# Create configuration directories
mkdir -p /etc/neoinit/services
mkdir -p /var/log/neoinit

# Copy configuration examples
cp examples/neoinit.conf /etc/neoinit/
cp examples/services/* /etc/neoinit/services/
```

### 2. System Integration
For LFS systems, add to your boot scripts:
```bash
# /etc/inittab or equivalent
id:3:initdefault:

# /etc/rc.d/init.d/rcS
#!/bin/sh
/sbin/neoinit
```

### 3. Service Configuration Example
```ini
[Service]
Name=example
Description=Example Service
ExecStart=/usr/bin/example
Type=simple
Restart=always

[Dependencies]
Requires=network.service
After=network.service

[Resources]
MemoryLimit=256M
CPUWeight=100
```

## Troubleshooting

### 1. Common Issues

#### Service Won't Start
```bash
# Check logs
tail -f /var/log/neoinit/service.log

# Check status
neoinit status service_name

# Check dependencies
neoinit check-deps service_name
```

#### Compilation Issues
```bash
# Clean
make clean

# Rebuild with debug info
make debug
```

### 2. Diagnostics

#### System Check
```bash
# Check configuration
neoinit check-config

# Verify file integrity
neoinit verify-installation
```

### 3. FAQ

**Q:** Can I install Neoinit on Ubuntu/Debian/Fedora?  
**A:** No. Neoinit is designed exclusively for LFS-based systems, independent distributions, and NurOS.

**Q:** How to integrate Neoinit with LFS?  
**A:** Follow the LFS book section on boot scripts and replace SysVinit/SystemD configurations with Neoinit.

**Q:** Will Neoinit work with my existing services?  
**A:** You'll need to convert your service configurations to Neoinit format. Use the provided examples as templates.

## Additional Information

### Documentation
- [API Documentation](API.md)
- [Contributing Guide](CONTRIBUTING.md)
- [Security Policy](SECURITY.md)

### Support

If you encounter any issues:
1. Check the [FAQ](https://github.com/nuros-linux/neoinit/wiki/FAQ)
2. Create an [Issue](https://github.com/nuros-linux/neoinit/issues)
3. Join IRC: #nuros on EFnet

---

*Last updated by [AnmiTaliDev](https://github.com/AnmiTaliDev) on 2024-12-21 08:00:36 UTC*
