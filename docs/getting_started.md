## Dependencies

A C++17 compatible compiler and CMake version >= 3.19 are required.

Depending on which component you want to build, the following dependencies must be installed, for example through your package manager or by building them from source.
An instruction to install all dependencies with Ubuntu 22.04 is given below.

Some dependencies are already included in the SOLA repository and are not listed here.

**Be advised that DAISI depends on all other components and all components depend on SolaNet.**


| External dependency | DAISI                   | MINHTON                | natter | SolaNet               |
|---------------------|:-----------------------:|:----------------------:|:------:|:---------------------:|
| libyaml             | :fontawesome-solid-x:   | :fontawesome-solid-x:  |        |                       |
| ns-3 (>= 3.36)      | :fontawesome-solid-x:   |                        |        |                       |
| SQLite3             | :fontawesome-solid-x:   |                        |        |                       |
| libuuid             |                         |                        |        | :fontawesome-solid-x: |
| libevent            |                         | :fontawesome-solid-x:  |        |                       |

:fontawesome-solid-x: means that this dependency is required from the specified component.

### Install dependencies on Ubuntu 22.04

To install all dependencies that are available through ``apt`` (all except ns-3) run:

```sh
apt install libyaml-cpp-dev libsqlite3-dev uuid-dev libevent-dev
```

To install ns-3, make sure that the prerequires are met as described in the [ns-3 documentation](https://www.nsnam.org/documentation/).
Download and extract the allinone package of ns-3 **version 3.36 or later** from [here](https://www.nsnam.org/releases/latest/).
Enter the folder ``ns-allinone-3.XX/ns-3.XX`` (replace XX with the version number you have downloaded) and run the following commands.
Replace ``<INSTALL_PREFIX>`` with a path you wish to install ns-3 to.

```sh
./ns3 configure --prefix <INSTALL_PREFIX>
./ns3 install
```

This will build all of ns-3 and install it into ``<INSTALL_PREFIX>``.

## Build from Terminal
Checkout the repository with its submodules, enter it and create a build folder:

```sh
git clone https://github.com/iml130/sola.git --recurse-submodules
cd sola
mkdir build && cd build
```

There are two ways to invoke CMake to generate the buildfiles:

### Build without ns-3 integration
Pass the ``{REPOSITORY_ROOT}`` folder to CMake as a source directory.

```
cmake ..
cmake --build .
```

This will build everything excluding the ns-3 integration.
To build single components only you can specify ``--target <TARGET_NAME>``.
For example to only build the MINHTON library (and all its internal dependencies), call

```
cmake --build . --target minhton_core_node
```

### Build with ns-3 integration

Pass the ``{REPOSITORY_ROOT}/daisi`` folder to CMake as a source directory.
Make sure to pass ``-Dns3_DIR="<NS_3_INSTALL_PREFIX>/lib/cmake/ns3"`` so that CMake can find ns-3.

```sh
cmake ../daisi -Dns3_DIR="<NS_3_INSTALL_PREFIX>/lib/cmake/ns3"
cmake --build .
```

This will build everything.
With this way it is also possible to build the same components as if building without ns-3 integration.

## Build with Visual Studio Code

This section describes how to build SOLA with ns-3 integration using the **CMake Tools** Visual Studio Code extension.

1. Checkout the repository including the submodules.
2. Install the [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extension for VS Code.
3. Add `"cmake.configureSettings": {"ns3_DIR": "<NS_3_INSTALL_PREFIX>/lib/cmake/ns3"},` to `${REPOSITORY_ROOT}/.vscode/settings.json`.
Replace ``<NS_3_INSTALL_PREFIX>`` with the path to your ns-3 installation.
4. Execute ``CMake: Configure`` via the VS Code command palette (``CTRL+SHIFT+P``) to configure the project. You can change the ``CMAKE_BUILD_TYPE`` in the status bar.
5. Build the project by either calling ``CMake: Build`` or pressing ``CTRL+SHIFT+B``.

In case VS Code shows the problem of a too large workspace, the file limit has to be [adjusted in /etc/sysctl.conf with root rights](https://code.visualstudio.com/docs/setup/linux#_visual-studio-code-is-unable-to-watch-for-file-changes-in-this-large-workspace-error-enospc).
This problem only appears on [Linux kernels prior to version 5.11 or low-performance systems](https://github.com/torvalds/linux/commit/92890123749bafc317bbfacbe0a62ce08d78efb7).

## Using the components

To execute a ns-3 simulation or integrating components (SOLA, MINHTON, natter) into your application, visit the following pages:

* [Execute DAISI simulation](daisi/using.md)
* [Using SOLA](sola/usage.md)
* [Using MINHTON](management_overlay/using.md)
* [Using natter](natter/using.md)
