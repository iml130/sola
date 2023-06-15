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
