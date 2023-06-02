# Installation Manual

After the [required dependencies](#required-dependencies) are met, you can [build DAISI](#build-daisi) and [other components](#build-single-libraries).

## Required dependencies

In any case, a C++17 compatible compiler, e.g., GCC >= 8, and CMake >= 3.19 are required.

Loaded as packages in CMake (`find_package`):

| Dependency | DAISI | MINHTON | natter & SolaNet |
|------------|:-----:|:-------:|:----------------:|
| pthreads support            | ★ | ★ | ★ |
| [libyaml](#install-libyaml) | ★ | ★ |   |
| [ns-3](#install-ns-3)       | ★ |   |   |
| [SQLite3](#install-sqlite3) | ★ |   |   |

Loaded as libraries in CMake (`find_library`):

| Dependency | natter | MINHTON |
|------------|:------:|:-------:|
| [libuuid](#install-libuuid)   | ★ | ★ |
| [libevent](#install-libevent) | ★ |   |

A ★ means that this dependency is required from the specified component.

### Install ns-3

Please refer to the [ns-3 documentation](https://www.nsnam.org/docs/installation/html/quick-start.html) and make sure the ns-3 prerequisites are met.

Download the current [ns-3 version](https://www.nsnam.org/releases/latest/).
Then, build and install ns-3.
You may do all of this in the following way, exemplarily shown for ns-3 version 3.38 on Ubuntu:

```sh
apt install g++ python3 cmake ninja-build git ccache
apt install wget bzip2
wget -N https://www.nsnam.org/releases/ns-allinone-3.38.tar.bz2
tar xjf ns-allinone-3.38.tar.bz2
cd ns-allinone-3.38
cmake ns-3.38/ -DCMAKE_BUILD_TYPE=Release -DNS3_ENABLED_MODULES="core;mobility;applications;network;bridge;csma;wifi;netsimulyzer" -DCMAKE_INSTALL_PREFIX=/work/ns3/ns3_installs/ns3_3_38_release_stripped
make && make install
```

Feel free to change the parameter for the `DCMAKE_INSTALL_PREFIX` option that specifies the installation location of ns-3, but keep in mind to adjust the later commands in this case.

### Install other dependencies

<a name="install-libyaml"></a>
<a name="install-sqlite3"></a>
<a name="install-libuuid"></a>
<a name="install-libevent"></a>

If apt is available on your distribution, you can install the other dependencies with:

```sh
apt install libyaml-cpp-dev libsqlite3-dev uuid-dev libevent-dev
```

Otherwise, please use `dpkg`, `pacman` or install the dependencies from sources.
Of course, you can also install only the required dependencies for the specific library if you don't want to build DAISI.

## Build DAISI

First you need to checkout the repository with its submodules and open it:

```sh
git clone git@github.com:iml130/sola.git --recurse-submodules
cd sola
```

Afterwards you can build DAISI with one of the following options:

### 1. Terminal

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

### 2. Visual Studio Code

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

### Next Steps

After DAISI was successfully built, you can check out the [Getting Started Guide](sola/index.md).

## Build single libraries

If you only want to build a single library, you can just navigate to the respective folder and build it with CMake.
For example, building natter as a library works like this:

```sh
cd natter
mkdir build && cd build
cmake ..
cmake --build .
```
