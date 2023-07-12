# SOLA

![SOLA](./docs/img/sola_dark.png#gh-dark-mode-only)
![SOLA](./docs/img/sola_light.png#gh-light-mode-only)

**SOLA** is decentralized communication middleware that omits the need for a dedicated communication instance.
It operates on peer's local view, where each peer maintains selected links to a subset of all peers.
The local view of a peer is used for its two main building blocks.
The management overlay is a tree-structured peer to peer network that supports a service and peer discovery mechanism and an event dissemination, which enables a broadcast operation with the publish-subscribe message pattern.

**DISCLAIMER**:
This project is not intended for everyday use and made available without any support.
However, we welcome any kind of feedback via the issue tracker or by e-mail.

## Project structure and licensing

This repository contains multiple subprojects which are published under different licenses.
The top-level folder of this repository is licensed under the MIT license.

| Folder     | SPDX Identifier | Description                                                         |
| ---------- | ----------------| ------------------------------------------------------------------- |
| daisi      | GPL-2.0-only    | ns-3 network simulation setup for all components and applications   |
| minhton    | MIT             | P2P tree-structured overlay network                                 |
| natter     | MIT             | P2P broadcasting framework with broadcasting algorithm for MINHTON  |
| sola       | MIT             | Decentralized communication middleware                              |
| solanet    | MIT             | Networking utils for other components                               |
| evaluation | MIT             | Verification/Validation/Evaluation scrips                           |

This project uses third party software which might be licensed differently.
For the licenses of third party software see the license files in the corresponding folders.

## Build

If you want to build DAISI, you need to have ns-3 and some additional dependencies installed.
Then, you can build DAISI from a build subdirectory as a CMake project:

```sh
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -Dns3_DIR="PATH_TO_YOUR_NS3_INSTALL"
cmake --build . --target Main
```

Please refer to the [documentation](https://iml130.github.io/sola/) for an in depth installation guide including other options as well as further information.

## Contributing

If you want to get involved, feel free to ask questions, bring ideas to discuss, or anything else.
We are open-source and we like open thoughts!

See ``CONTRIBUTING.md`` for further information.

## Academic Attribution

If you use it for research, please include the following reference in any resulting publication.

```plain
@inproceedings{detznerSOLADecentralizedCommunication2023,
  title = {{{SOLA}}: {{A Decentralized Communication Middleware Developed}} with ns-3},
  shorttitle = {{{SOLA}}},
  booktitle = {Proceedings of the 2023 {{Workshop}} on Ns-3},
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

SOLA is a composition of other components. 
References to the publications are provided in the dedicated documentation.