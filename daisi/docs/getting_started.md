# Getting started

## Quickstart (if we want to)

- Install, build and run cpps on Ubuntu 22.04 or using apt
- find logfile

- **TO BE TESTED**

### Install dependencies

ns-3

```sh
apt install g++ python3 cmake ninja-build git ccache
apt install wget bzip2
wget -N https://www.nsnam.org/releases/ns-allinone-3.38.tar.bz2
tar xjf ns-allinone-3.38.tar.bz2
cd ns-allinone-3.38
cmake ns-3.38/ -DCMAKE_BUILD_TYPE=Release -DNS3_ENABLED_MODULES="core;mobility;applications;network;bridge;csma;wifi;netsimulyzer" -DCMAKE_INSTALL_PREFIX=/work/ns3/ns3_installs/ns3_3_38_release_stripped
make && make install
```

Other dependencies

```sh
apt install libyaml-cpp-dev libsqlite3-dev uuid-dev libevent-dev
```

### Build

First you need to checkout the repository with its submodules and open it:

```sh
git clone git@github.com:iml130/sola.git --recurse-submodules
cd sola
```

Build DAISI as a normal CMake project.
Make sure to pass ``-Dns3_DIR="PATH_TO_YOUR_NS3_INSTALL"`` so that CMake can find ns3.
The `PATH_TO_YOUR_NS3_INSTALL` should look similar to `/work/ns3/ns3_installs/ns3_3_38_release_stripped/lib/cmake/ns3`.

```sh
mkdir build && cd build
cmake ../daisi -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -Dns3_DIR="PATH_TO_YOUR_NS3_INSTALL"
cmake --build . --target Main
```

Then, the main executable can be found at `BUILD_FOLDER/src/main/Main`.
The default `BUILD_FOLDER` is `build/daisi/` inside the folder of the repository.
We recommend copying the executable to the root folder of the repository for easier access:

```sh
cp daisi/src/main/Main ../daisi_exec
```

A requirement is to set the environment variables, at least the `LD_LIBRARY_PATH`, while running cpps or sola (standalone) additionally require setting the `MINHTONDIR`:

```sh
export LD_LIBRARY_PATH=/work/ns3/ns3_installs/ns3_3_38_release_stripped/lib
export MINHTONDIR=`pwd`/minhton
```

### Run

```sh
./daisi_exec --environment=cpps --scenario=daisi/scenarios/cpps/default.yml
```

- Read the logfile
    - find filename
    - open with sqlite
    - open with 


## Required dependencies

- Refer to [Required dependencies](../../docs/index.md#required-dependencies).

### Install ns-3

Please refer to the [ns-3 documentation](https://www.nsnam.org/documentation/) and make sure the ns-3 prerequisites are met.

## Build 

First you need to checkout the repository with its submodules and open it:

```sh
git clone git@github.com:iml130/sola.git --recurse-submodules
cd sola
```

Afterwards you can build DAISI with one of the following options:

### Terminal

Build DAISI as a normal CMake project.
Make sure to pass ``-Dns3_DIR="PATH_TO_YOUR_NS3_INSTALL"`` so that CMake can find ns3.
The `PATH_TO_YOUR_NS3_INSTALL` should look similar to `/work/ns3/ns3_installs/ns3_3_38_release_stripped/lib/cmake/ns3`.

```sh
mkdir build && cd build
cmake ../daisi -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -Dns3_DIR="PATH_TO_YOUR_NS3_INSTALL"
cmake --build . --target Main
```

Then, the main executable can be found at `BUILD_FOLDER/src/main/Main`.
The default `BUILD_FOLDER` is `build/daisi/` inside the folder of the repository.
We recommend copying the executable to the root folder of the repository for easier access:

```sh
cp daisi/src/main/Main ../daisi_exec
```

If you want to build other targets than DAISI you can replace the `target` parameter with another target or `all` if you want to build everything.

### Visual Studio Code

<!--We provide some files that makes it easy to build, run and debug DAISI with [Visual Studio Code](https://code.visualstudio.com/).

1. Open your sola-ns3/DAISI folder within VS Code.
2. Run ``vscode.py``. It will ask you, which installed ns3 version should be used (searching at ``/work/ns3/ns3_installs``) and which generator to use.
5. Easily select the debug configuration and press F5 - happy debugging :)-->

1. Install the [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extension for VS Code.
2. Add `"cmake.configureSettings": {"ns3_DIR": "PATH_TO_YOUR_NS3_INSTALL"},` to a `settings.json` from VS Code. The `PATH_TO_YOUR_NS3_INSTALL` should look similar to `/work/ns3/ns3_installs/ns3_3_38_release_stripped/lib/cmake/ns3`.
3. Execute ``CMake: Configure`` via the VS Code command palette (``CTRL+SHIFT+P``) to configure the project. You can change the ``CMAKE_BUILD_TYPE`` in the status bar.
4. Build the project by either calling ``CMake: Build`` or pressing ``CTRL+SHIFT+B``.

In case VS Code shows the problem of a too large workspace, the file limit has to be [adjusted in /etc/sysctl.conf with root rights](https://code.visualstudio.com/docs/setup/linux#_visual-studio-code-is-unable-to-watch-for-file-changes-in-this-large-workspace-error-enospc).
This problem only appears on [Linux kernels prior to version 5.11 or low-performance systems](https://github.com/torvalds/linux/commit/92890123749bafc317bbfacbe0a62ce08d78efb7).

## Using Environments

- Several environments can be built and used. 
    - [SOLA](./sola-ns3/using.md)
    - [MINHTON](./minhton-ns3/using.md)
    - [NATTER](./natter-ns3/using.md)
    - [OptiMaFlow](./optimaflow-ns3/using.md)
