# System.af Documentation

## Overview
`System.af` is a part of the Aflat standard library, providing functionalities for system-level operations and environment management. This file includes functions for executing system commands, managing environment variables, and handling concurrent processes.

## Functions

### `execve`
Executes a program specified by the given path.

- **Signature**: `int execve(const adr path, * const adr argv, * adr envp)`
- **Parameters**:
  - `path`: Address of the program to execute.
  - `argv`: Pointer to an array of argument strings.
  - `envp`: Pointer to an array of environment variables.
- **Returns**: The return status of the executed program.

### `execWorker`
Executes a worker process.

- **Signature**: `adr execWorker(const adr _arg) : async.worker`
- **Parameters**:
  - `_arg`: Address of the argument array.
- **Returns**: NULL after execution.

### `exec`
Executes a program and waits for its completion.

- **Signature**: `int exec(const adr path, * const adr argv, * const adr envp)`
- **Parameters**:
  - `path`: Address of the program to execute.
  - `argv`: Pointer to an array of argument strings.
  - `envp`: Pointer to an array of environment variables.
- **Returns**: The exit status of the executed program.

### `system`
Executes a shell command.

- **Signature**: `int system(const adr cmd)`
- **Parameters**:
  - `cmd`: Address of the command string to execute.
- **Returns**: The exit status of the shell command.

### `envVar`
Retrieves the value of an environment variable.

- **Signature**: `adr envVar(const adr name)`
- **Parameters**:
  - `name`: Address of the name of the environment variable.
- **Returns**: Address of the value of the environment variable.

### `isSet`
Checks if an environment variable is set.

- **Signature**: `bool isSet(const adr name)`
- **Parameters**:
  - `name`: Address of the name of the environment variable.
- **Returns**: `true` if the variable is set, `false` otherwise.

## Usage
The functions in `System.af` are essential for performing system-level operations in the Aflat programming language. They provide the capability to execute external programs, manage environment variables, and handle system commands, making them crucial for applications that require interaction with the underlying operating system.
