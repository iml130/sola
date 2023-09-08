# Introduction

<p align="center">
<img src="img/natter_Logo_separated.svg" class="logo" width="200px" alt="natter Logo">
</p>

**natter** is a topic-based publish/subscribe framework for decentralized Pub-Sub by using peer-to-peer connections.
It allows sending a message to all nodes interested in a particular topic using unicast messages between these nodes.
***natter*** allows implementing different algorithms to specify how every node should forward a message to other nodes.
These algorithms are called broadcast algorithms because they broadcast the message to all nodes in the group of interested participants.
The management of the node's topic interests and how the nodes are interconnected must be handled by the user.
Connected nodes are passed to the broadcast algorithm instances together with the corresponding topic and are used for message dissemination.
Hence ***natter*** is network-structure agnostic and could support structures like rings, grids or trees by providing a suitable algorithm. 

We used ***natter*** to develop the **MINHCAST** algorithm.
MINHCAST uses the **MINHTON** tree structure.
MINHCAST forwards the message to all nodes of a MINHTON tree deterministically in a way, that every node receives the message only once.
The MINHCAST algorithm is described [here](programmers/minhcast.md).

With this library, we address the following design goals:

- Nodes can subscribe to a specific topic and receive messages
- Publish a (broadcast) message to a topic using point-to-point connections, e.g., UDP
- Ease the development of broadcast algorithms for different network-structures
- Supporting up to millions of nodes - in theory ;-)

For a practical example or instructions how to develop your own algorithm for your P2P network structure, visit [Using natter](using.md).

