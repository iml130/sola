# Introduction to SOLA

![SOLA Logo](img/sola_light.png#gh-light-mode-only)
![SOLA Logo](img/sola_dark.png#gh-dark-mode-only)

This documentation aims to give as much information as possible to give an overview how SOLA is working and how to use it.
Detailed information about how to [setup](installation.md) and [run SOLA](sola/index.md) is available in this documentation.

This documentation is currently incomplete. Any feedback on this documentation is highly welcome, including bug reports,
typos or stuff you think should be included but is not. Thanks in advance!

## What is SOLA?

SOLA is decentralized communication middleware that omits the need for a dedicated communication instance.
It operates on peer's local view, where each peer maintains selected links to a subset of all peers.
The local view of a peer is used for its two main building blocks.
The _management overlay_ is a tree-structured peer to peer network that supports a service and peer discovery mechanism and an _event dissemination_, which enables a broadcast operation with the publish-subscribe message pattern.

_A distributed system is a collection of independent computers that appears to its users as a single coherent system._ [[1]](#references)

## How does it work?

SOLA is an extensive, modular middleware completely written in C++ consisting of multiple components and applications, namely DAISI, MINHTON, natter, SOLA (Standalone), and SOLANET:

- Decentralized trAnsport Intralogistic SImulation (**DAISI**) provides a configuration for the network simulator ns-3 and includes the applications
    - Cyber-Physical Production System (**[CPPS](daisi_lib/applications/cpps/index.md)**), and
    - **[Path Planning](daisi_lib/applications/path-planning/index.md)**.
- MINimal Height Tree Overlay Network (**[MINHTON](minhton_lib/index.md)**) is a peer-to-peer management overlay which organizes peers in an optimized tree structure.
- **[natter](natter_lib/index.md)** is a broker-less publish-subscribe library for event dissemination that provides the broadcasting algorithm MINHCAST for MINHTON.
- **SOLA (Standalone)** is the component that initializes and uses MINHTON as well as natter. SOLA can be used as the middleware for all communication purposes.
- **SOLANET** is a basic asynchronous UDP networking library for establishing connections between the peers.

Together, they form the decentralized communication middleware SOLA.

We are using **[ns-3](ns-3/index.md)**, a discrete-event network simulator for Internet systems as the simulation environment for SOLA.
ns-3 is targeting primarily research and educational purpose.
However, we are using it for the development of an Industry 4.0 application and communication technologies.
The only other dependencies are the unit testing framework **Catch2** and the serialization library **cereal**.

The entry point for running the simulation is located in [daisi/src/main/daisi_main.cpp](https://github.com/iml130/sola/blob/main/daisi/src/main/daisi_main.cpp).

## Why?

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

## References

[1] M. van Steen and A. S. Tanenbaum, [Distributed systems](https://www.distributed-systems.net/index.php/books/ds3/), Third edition (Version 3.01 (2017)). London: Pearson Education, 2017.
