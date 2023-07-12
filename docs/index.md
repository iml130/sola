<p align="center">
<img src="img/sola_light.png" class="logo" width="300px" alt="natter Logo">
</p>


This documentation aims to give as much information as possible to give an overview how SOLA works and how to use it.
Detailed information about how to [setup](installation.md) and [run SOLA](sola/index.md) is available in this documentation.

This documentation is currently incomplete. Any feedback on this documentation is highly welcome, including bug reports,
typos or stuff you think should be included but is not. 
Thanks in advance!

## What is SOLA?

SOLA is decentral organized communication middleware that omits the need for a dedicated communication instance.
Contrary to other communication middlewares such as [MQTT](https://mqtt.org/), [ROS 1](https://www.ros.org/), [OPC UA](https://opcfoundation.org/about/opc-technologies/opc-ua/), or [Data Distribution Service (DDS)](https://www.dds-foundation.org/what-is-dds-3/), each node in SOLA operates on a local limited view where *O(n) while n << N*.
The local view is of each peer maintains selected links to a subset of all peers in its routing table (RT).

In the following Figure 1, each peer within SOLA is represented by a single entity. 
Furthermore, the nodes are required to collaborate in order to appear to its user as a single coherent system
![SOLA](img/sola_app.svg)
**Figure 1**: SOLA appears to it users as a single coherent system. As each node in SOLA has a limited view, a collaboration among all nodes is required.

Entities, such as automated guided vehicles (AGVs), autonomous mobile robots (AMRs) or even virtual entities such as material flow or a production order, can join and become part of SOLA.
SOLA is an extensive, modular middleware completely written in C++ and its main purpose as a communication middleware is, to enable nodes to communicate with each other.
This requires to publish events through event dissemination (event producer) or express their interest (event consumer) by subscribing to specific events. 
Moreover, nodes can search for services, peers, or data that fulfill specified conditions. This enables a loose coupling
during operational runtime.
The local view of a peer is used for its two main building blocks, as depicted in the following Figure 2.

![SOLA](img/sola_overview.svg)
**Figure 2**: SOLA's main building blocks.

* The *_Management Overlay_*  is a tree-structured peer to peer network that supports a service and peer discovery mechanism
* The *_Event Dissemination_*  enables a broadcast operation with the publish-subscribe message pattern.


## Why decentral organized communication?

Industry 4.0 (I4.0) is the current ongoing fourth industrial revolution.
Key-pillars of the I4.0 are interconnectivity, information transparency, decentralized decision-making, and technical assistance.
Interconnectivity means that a wide variety of machines, sensors and devices can communicate and interact with each other.
Standardized interfaces are used for this purpose.
Through the aforementioned interconnectivity, the real world is unified with the virtual world.
Exchanged information can be put into context.
Context-related information is the basis for further decision-making.
Decentralized decision-making is based on networking and information transparency.
Local information is linked with global information.
Participants act on this aggregated information.
Technical assistance ensures that the increasingly complex processes remain manageable.
This support is particularly important for humans, since their role as machine operators is evolving into that of problem solvers.
 

## Project Structure

**TODO: Explanaition of the horizontal bullet points, move content to main** 

<!--
TODO: Overview DAISI + SOLA + remaining components --> 
 -->
- **SOLA (Standalone)** is the component that initializes and uses MINHTON as well as natter. SOLA can be used as the middleware for all communication purposes.
- minhton: MINimal Height Tree Overlay Network (**[MINHTON](minhton_lib/index.md)**) is a peer-to-peer management overlay which organizes peers in an optimized tree structure. 
- natter

Together, they form the decentralized communication middleware SOLA.

- **D**ecentralized tr**A**nsport **I**ntralogistic **SI**mulation (**DAISI**) provides a configuration for the network simulator ns-3 and includes the applications
    - Intralogistic (**[SOMF](daisi_lib/applications/cpps/index.md)**), and
    - **[Path Planning](daisi_lib/applications/path-planning/index.md)**.
- **SOLANET** is a basic asynchronous UDP networking library for establishing connections between the peers.


We are using **[ns-3](ns-3/index.md)**, a discrete-event network simulator for Internet systems as the simulation environment for SOLA.
ns-3 is targeting primarily research and educational purpose.
However, we are using it for the development of an Industry 4.0 application and communication technologies.

The entry point for running the simulation is located in [daisi/src/main/daisi_main.cpp](https://github.com/iml130/sola/blob/main/daisi/src/main/daisi_main.cpp).




## Getting Started

<!-- TODO: elaborate here ... -->
SOLA depends on few requirements.

### Required dependencies

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

### SOLA stand-alone

After the [required dependencies](#required-dependencies) are met, you can [build DAISI](#build-daisi) and [other components](#build-single-libraries).

### SOLA with ns-3

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

### Build 

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

### Next Steps

After DAISI was successfully built, you can check out the [Getting Started Guide](sola/index.md).

### Build single libraries

If you only want to build a single library, you can just navigate to the respective folder and build it with CMake.
For example, building natter as a library works like this:

```sh
cd natter
mkdir build && cd build
cmake ..
cmake --build .
```



## Contribution to the Documentation

SOLA is an open source project, and as such all contributions, both in the form of feedback and content generation, are most welcomed.
Keep in mind that this project is open source and licensed under the license specified in the repository.
To make such contributions, please refer to the [Contribution Guidelines](https://github.com/iml130/sola/blob/main/CONTRIBUTING.md) hosted in our GitHub repository.




## Academic Attribution

If you use SOLA for research (or any other purposes), please include one of the following references in any resulting publication.

```plain
@inproceedings{detznerSOLADecentralizedCommunication2023,
  title = {{{SOLA}}: {{A Decentralized Communication Middleware Developed}} with ns-3},
  shorttitle = {{{SOLA}}},
  booktitle = {Proceedings of the 2023 {{Workshop}} on ns-3},
  author = {Detzner, Peter and Gödeke, Jana and Tönning, Lars and Laskowski, Patrick and Hörstrup, Maximilian and Stolz, Oliver and Brehler, Marius and Kerner, Sören},
  date = {2023-06-28},
  series = {{{WNS3}} '23},
  pages = {78--85},
  publisher = {{Association for Computing Machinery}},
  location = {{New York, NY, USA}},
  url = {https://dl.acm.org/doi/10.1145/3592149.3592151},
  abstract = {The transformation from static production facilities into a flexible and decentralized cyber-physical production system (CPPS) is part of the current ongoing Industry 4.0. A CPPS will enable and support communication between people, machines and virtual objects, e.g., as material flow or products, alike. However, communication in CPPS relies often on centralized approaches using a message broker or is not considered at all. We present in this paper the decentralized communication middleware called SOLA with an emphasis on, but not limited to, CPPS. SOLA uses the inherent given capability to communicate of participating nodes and eliminates the need for a central instance. A structured overlay network is created and managed, which appears to its users as a single coherent system. The main building blocks of SOLA are the management overlay and the event dissemination. Within this building blocks, no single peer has a global view and all operations are based on each peer’s local view. The local view represents some selected links to a subset of all peers in the network. In addition to this, we also present how SOLA was developed with the help of the discrete-event simulator ns-3. Finally, we also show how we have used ns-3 to simulate a self-organizing material flow where participants use SOLA to communicate.},
  isbn = {9798400707476},
  keywords = {balanced tree,cyber-physical production system,decentral organized communication,peer-to-peer network}
}
```