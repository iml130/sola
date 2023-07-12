<p align="center">
<img src="img/natter_Logo_separated.svg" class="logo" width="200px" alt="natter Logo">
</p>

**natter** is a fully decentralized topic-based publish/subscribe framework.
It allows sending a message to all nodes interested in a particular topic using unicast messages between these nodes.
***natter*** allows implementing different algorithms to specify how every node should forward a message to other nodes.
These algorithms are called broadcast algorithms because they broadcast the message to all nodes in the group of interested participants.
The management of the node's topic interests and how the nodes are interconnected must be handled by the user.
Connected nodes are passed to the broadcast algorithm instances together with the corresponding topic and are used for message dissemination.
Hence ***natter*** is network-structure agnostic and could support structures like rings, grids or trees by providing a suitable algorithm. 

We used ***natter*** to develop the **MINHCAST** algorithm.
***MINHCAST*** uses the **MINHTON** tree structure, which is based on the nBATON\* structure [1].
For every topic, a separate MINHTON tree is created.
***MINHCAST*** forwards the message to all nodes of a MINHTON tree deterministically in a way, that every node receives the message only once.
A detailed description of ***MINHCAST*** will be provided soon.

With this library, we address the following design goals:

- Nodes can subscribe to a specific topic and receive messages
- Publish a (broadcast) message to a topic using point-to-point connections, e.g., UDP
- Ease the development of broadcast algorithms for different network-structures
- Supporting up to millions of nodes - in theory ;-)

A good starting point to get familiary with the library is the chat example at ``examples/chat``.

## References
[1] Detzner, Peter, Jana Gödeke, and Steffen Bondorf. “Low-Cost Search in Tree-Structured P2P Overlays: The Null-Balance Benefit.” In 2021 IEEE 46th Conference on Local Computer Networks (LCN) (LCN 2021). Edmonton, Canada, 2021.
[https://doi.org/10.1109/LCN52139.2021.9525004](https://doi.org/10.1109/LCN52139.2021.9525004)
