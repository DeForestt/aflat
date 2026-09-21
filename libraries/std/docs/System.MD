# System.af Documentation

## Overview
`System.af` is a part of the Aflat standard library, providing functionalities for system-level operations and environment management. This file includes functions for executing system commands, managing environment variables, and handling concurrent processes.

## Functions

### `execve`
Replaces the current process image with the specified program. On success it
does not return; on failure it returns the underlying syscall result.

- **Signature**: `int execve(const adr path, * const adr argv, * adr envp)`
- **Parameters**:
  - `path`: Address of the program to execute.
  - `argv`: Pointer to an array of argument strings.
  - `envp`: Pointer to an array of environment variables.
- **Returns**: A syscall error on failure; no return on successful replacement.

### `execWorker`
Executes a worker process.

- **Signature**: `any execWorker(const adr _arg) : async.worker::<adr>(WorkerMode.PROCESS)`
- **Parameters**:
  - `_arg`: Address of the argument array.
- **Behavior**: Process-worker trampoline for the legacy concurrency API,
  distinct from a native `task<T>`.

### `exec`
Executes a program and waits for its completion.

- **Signature**: `int exec(const adr path, * const adr argv, * const adr envp)`
- **Parameters**:
  - `path`: Address of the program to execute.
  - `argv`: Pointer to an array of argument strings.
  - `envp`: Pointer to an array of environment variables.
- **Returns**: The raw wait status written by `sys_wait`, not a decoded exit code.

### `system`
Executes a shell command.

- **Signature**: `int system(const adr cmd)`
- **Parameters**:
  - `cmd`: Address of the command string to execute.
- **Returns**: The raw wait status from `exec`.

### `envVar`
Retrieves the value of an environment variable.

- **Signature**: `adr envVar(const adr name)`
- **Parameters**:
  - `name`: Address of the name of the environment variable.
- **Returns**: A borrowed environment address, or NULL if missing. Do not free
  it or assume a copy of the bytes was allocated.

### `isSet`
Checks if an environment variable is set.

- **Signature**: `bool isSet(const adr name)`
- **Parameters**:
  - `name`: Address of the name of the environment variable.
- **Returns**: `true` if the variable is set, `false` otherwise.

## Usage
The functions in `System.af` are essential for performing system-level operations in the Aflat programming language. They provide the capability to execute external programs, manage environment variables, and handle system commands, making them crucial for applications that require interaction with the underlying operating system.
