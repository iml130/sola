# Introduction to SOLA

![SOLA Logo](img/sola_light.png#gh-light-mode-only)
![SOLA Logo](img/sola_dark.png#gh-dark-mode-only)

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
 

## Academic Attribution

If you use SOLA for research (or any other purposes), please include one of the following references in any resulting publication.

```plain
@inproceedings{detznerSOLADecentralizedCommunication2023,
  title = {{{SOLA}}: {{A Decentralized Communication Middleware Developed}} with Ns-3},
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