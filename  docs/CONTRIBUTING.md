# Contributing to Neoinit 

## Table of Contents
1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [Development Process](#development-process)
4. [Coding Standards](#coding-standards)
5. [Pull Request Process](#pull-request-process)
6. [Security Policy](#security-policy)
7. [Community Guidelines](#community-guidelines)

## Code of Conduct

### Our Pledge

In the interest of fostering an open and welcoming environment, we pledge to make participation in our project and our community a harassment-free experience for everyone.

### Our Standards

- Be respectful and inclusive
- Accept constructive criticism
- Focus on what is best for the community
- Show empathy towards other community members

## Getting Started

### Prerequisites

```bash
# Required development tools
sudo apt install build-essential git clang-format cppcheck valgrind doxygen

# Clone the repository
git clone https://github.com/nuros-linux/neoinit.git
cd neoinit
```

### Building for Development

```bash
# Debug build
make debug

# Run tests
make test

# Static analysis
make analyze
```

## Development Process

### Branch Strategy

- `main`: Production-ready code
- `develop`: Main development branch
- `feature/*`: New features
- `bugfix/*`: Bug fixes
- `release/*`: Release preparation
- `hotfix/*`: Emergency fixes

### Commit Guidelines

```
type(scope): description

[optional body]

[optional footer]
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `test`: Adding/modifying tests
- `chore`: Maintenance tasks

Example:
```
feat(service): add socket activation support

Implement basic socket activation functionality for services
with automated cleanup on service shutdown.

Closes #123
```

## Coding Standards

### C Code Style

```c
// Function naming
int start_service(const char *service_name);

// Structure naming
typedef struct {
    char name[MAX_NAME_LENGTH];
    int value;
} service_config_t;

// Constants and macros
#define MAX_SERVICES 1024
#define LOG_ERROR(fmt, ...) log_message(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

// Error handling
if (result < 0) {
    LOG_ERROR("Operation failed: %s", strerror(errno));
    return -1;
}
```

### Code Format

Use provided `.clang-format`:
```bash
# Format your changes
make format
```

### Static Analysis

Run before submitting:
```bash
make analyze
```

## Pull Request Process

1. **Fork and Clone**
   ```bash
   git clone https://github.com/nuros-linux/neoinit.git
   cd neoinit
   git remote add upstream https://github.com/nuros-linux/neoinit.git
   ```

2. **Create Branch**
   ```bash
   git checkout -b feature/your-feature
   ```

3. **Make Changes**
   - Write code
   - Add tests
   - Update documentation
   - Run static analysis
   - Format code

4. **Commit Changes**
   ```bash
   git add .
   git commit -m "feat(scope): description"
   ```

5. **Push and Create PR**
   ```bash
   git push origin feature/your-feature
   ```

### PR Requirements

- [ ] Code follows style guidelines
- [ ] Tests added/updated
- [ ] Documentation updated
- [ ] Changelog updated
- [ ] Static analysis passed
- [ ] CI checks passed

## Security Policy

### Reporting Security Issues

Please report security issues to security@nuros-linux.org or via GitHub Security Advisories.

DO NOT create public issues for security vulnerabilities.

### Security Best Practices

- Check for buffer overflows
- Validate all inputs
- Use secure memory handling
- Implement proper privilege separation
- Follow principle of least privilege

## Community Guidelines

### Communication Channels

- GitHub Issues: Bug reports and feature requests
- GitHub Discussions: General discussion and questions
- IRC: #neoinit on libera.chat
- Matrix: #neoinit:matrix.org

### Issue Guidelines

1. **Bug Reports**
   ```markdown
   ### Description
   Clear description of the issue

   ### Steps to Reproduce
   1. Step one
   2. Step two
   3. Step three

   ### Expected Behavior
   What should happen

   ### Actual Behavior
   What actually happens

   ### System Information
   - Neoinit Version:
   - OS:
   - Architecture:
   ```

2. **Feature Requests**
   ```markdown
   ### Feature Description
   Clear description of the proposed feature

   ### Use Case
   Why this feature is needed

   ### Proposed Solution
   How you think this could be implemented
   ```

## Recognition

Contributors will be credited in:
- CONTRIBUTORS.md file
- Release notes
- Project documentation

## Questions?

Feel free to:
- Open a discussion
- Join our IRC channel
- Contact maintainers

---

Thank you for considering contributing to Neoinit! Your efforts help make this project better for everyone.