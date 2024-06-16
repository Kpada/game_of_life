# Convey's Game of Life for STM32

![Build Status](https://github.com/kpada/game_of_life/actions/workflows/main.yaml/badge.svg)

This is a simple implementation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) for
microcontrollers.

It also sets up a comprehensive development environment in VSCode, which includes a Docker-based container with all
necessary tools, compilers, and extensions. This Setup can be used as a reference for similar projects, providing a
fully-featured development environment for efficient and streamlined development. For more details on setting up the
development environment, see the [Setup](#setup) section.

## Setup

### Development Container

VSCode allows using a container as a full-featured development environment.

This repository provides a [Docker development container](.devcontainer/Dockerfile) and its
[configuration](.devcontainer/devcontainer.json). The container includes all necessary tools, compilers, and VSCode
extensions for the entire development cycle.

For more information, refer to:

- The [Developing inside a Container](https://code.visualstudio.com/docs/devcontainers/containers) manual for general
  information.
- The
  [Sharing GPG Keys](https://code.visualstudio.com/remote/advancedcontainers/sharing-git-credentials#_sharing-gpg-keys)
  guide if you want to use [GPG](https://www.gnupg.org/) to sign your commits.

### Makefile

The [Makefile](Makefile) provides two set of commands for managing the project:

- **From the Development Container**: These are the primary commands used for development. They assume that you are
  working within the container, which contains all the necessary tools and environment configurations.

  For example, to build the project:

  ```sh
  make build
  ```

  This command builds the project within the container environment.

- **From the Host Machine**: These commands are useful for quick checks or integration with CI/CD pipelines. They run
  the specified tasks within the Docker container and then exit.

  For example, to build the project from the host machine:

  ```sh
  make container-build # Equivalent to `make container CMD="make build"`
  ```

  This command starts the container, builds the project, and then stops the container.

### Hardware Requirements

- [STM32F103C8](https://www.st.com/en/microcontrollers/stm32f103c8.html)
- [ST-LINK/V2](https://www.st.com/en/development-tools/st-link-v2.html)

### Libraries Used

The project uses the following libraries, which will be automatically fetched by CMake:

- [libopencm3](https://github.com/libopencm3/libopencm3)
- [GoogleTest](https://github.com/google/googletest)

## Build Instructions

CMake is used as the build system for this project. The following commands can be used to build the project:

- From the development container:

  ```sh
  make build
  ```

- From the host machine:

  ```sh
  make container-build
  ```

## Flashing Firmware

The project uses OpenOCD to flash the firmware onto the board. Use the following commands to flash the firmware onto the
STM32 board:

- From the development container:

  ```sh
  make flash
  ```

- From the host machine:

  ```sh
  make container-flash
  ```

## Debugging

The development container includes all the required tools and VSCode extensions for debugging. It also contains the
appropriate version of OpenOCD to support STM32 flashing and debugging.

To start debugging:

- Open the `Run and Debug` menu in VSCode.
- Select the appropriate debug configuration:
  - ST-Link: To build the firmware, flash the board, and start debugging.
  - Tests: To run the unit tests.
- Start the debugging session.

## Unit Tests

Unit tests are located in the `tests` directory. Use the following commands to run the tests:

- From the development container:

  ```sh
  make tests
  ```

- From the host machine:

  ```sh
  make container-tests
  ```

## Linting and Static Analysis

The project uses [pre-commit](https://pre-commit.com/) to enforce coding style and check for common errors. The full
list of checks can be found in the [.pre-commit-config.yaml](.pre-commit-config.yaml) config file.

The pre-commit hooks can be run with the following command:

- From the development container:

  ```sh
  make pre-commit
  ```

- From the host machine:

  ```sh
  make container-pre-commit
  ```

The project also uses [clang-tidy](https://clang.llvm.org/extra/clang-tidy.html), which is moved out of the pre-commit
hook. The [.clang-tidy](.clang-tidy) configuration file contains some basic checks and also enforces the
[Google C++ Style: Naming](https://google.github.io/styleguide/cppguide.html#Naming) conventions.

It can be run with the following command:

- From the development container:

  ```sh
  make clang-tidy
  ```

- From the host machine:

  ```sh
  make container-clang-tidy
  ```

## WSL Configuration

One could use Windows Subsystem for Linux (WSL) as the development environment.

Below are some additional hints for setting up WSL.

### Attaching ST-Link to WSL

To attach the ST-Link device to your WSL distribution, use the following PowerShell commands:

```powershell
$busid = (usbipd list | Select-String -Pattern 'STLink').Line.Split()[0]; usbipd wsl attach --busid $busid --distribution Ubuntu
```

### Configuring udev Rules for ST-LINK and USB-to-Serial Devices

To ensure proper permissions and functionality for ST-LINK and USB-to-Serial devices, you might need to configure udev
rules. This can be done by running the provided [udev.sh](tools/udev.sh) script on your WSL distribution:
