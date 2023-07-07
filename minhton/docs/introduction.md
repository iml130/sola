# Minimal Height Tree Overlay Network (MINHTON)

<p align="center">
<img src="img/minhtonLogo.svg" class="logo" width="200px" alt="MINHTON Logo">
</p>

## Abstract

Distributed systems, potentially growing large and consisting of heterogeneous nodes, are advised to be constructed following the Peer-to-Peer (P2P) networking paradigm, like the one in [Figure 1](#fig1).
It becomes imperative that a P2P network is paired with efficient protocols for each phase of its life cycle: construction as well as maintenance and operation.
Three operations are fundamental for a P2P network: nodes must be able to a) join, b) be located, c) leave.
The main challenge for efficient protocols is that a single node will only possess limited information about the network, also known as the local view.
The minimal height tree overlay network (MINHTON) is such a P2P overlay architecture featuring several beneficial structural properties added over existing tree-structured networks.
The minimal height guarantees a global tree balance, yet, it must be retained at all times, even though the P2P network may change dynamically.
MINHTON provides efficient protocols for node Join and Departure in logarithmic order, comparable to tree overlays with less strict structural guarantees, both retaining a minimal height tree.

![Figure 1](img/cpps.png "An interconnected cyber-physical production system, representing an P2P network")
<a name="fig1"></a>**Figure 1:** An interconnected cyber-physical production system, representing a P2P network.


## Academic Attribution

If you use it for research, please include one of the following references in any resulting publication.

```plain
@inproceedings{laskowskiTreestructuredOverlaysMinimal2023,
  title = {Tree-Structured {{Overlays}} with {{Minimal Height}}: {{Construction}}, {{Maintenance}} and {{Operation}}},
  shorttitle = {Tree-Structured {{Overlays}} with {{Minimal Height}}},
  booktitle = {Proceedings of the 17th {{ACM International Conference}} on {{Distributed}} and {{Event-based Systems}}},
  author = {Laskowski, Patrick and Detzner, Peter and Bondorf, Steffen},
  date = {2023-06-27},
  series = {{{DEBS}} '23},
  pages = {168--176},
  publisher = {{Association for Computing Machinery}},
  location = {{New York, NY, USA}},
  url = {https://dl.acm.org/doi/10.1145/3583678.3596894},
  abstract = {Distributed systems, potentially growing large and consisting of heterogeneous nodes, are advised to be constructed following the Peer-to-Peer (P2P) networking paradigm. It becomes imperative that a Peer-to-Peer (P2P) network is paired with efficient protocols for each phase of its life cycle: construction as well as maintenance and operation. Three operations are fundamental for a Peer-to-Peer (P2P) network: nodes must be able to a) join, b) be located, c) leave. The main challenge for efficient protocols is that a single node will only possess limited information about the network, also known as the local view. In this paper, we present the minimal height tree overlay network (MINHTON), a Peer-to-Peer (P2P) overlay architecture featuring several beneficial structural properties added over existing tree-structured networks. The minimal height guarantees a global tree balance, yet, it must be retained at all times, even though the Peer-to-Peer (P2P) network may change dynamically. MINHTON provides efficient protocols for node Join and Departure, both retaining a minimal height tree. We show that the operations achieve performance in logarithmic order, comparable to tree overlays with less strict structural guarantees.},
  isbn = {9798400701221},
  keywords = {balanced tree,overlay networks,peer-to-peer}
}
```

MINHTON is based on few other publications:

```plain
@inproceedings{detznerPeerDiscoveryTreeStructured2022,
  title = {Peer {{Discovery}} in {{Tree-Structured P2P Overlay Networks}} by {{Means}} of {{Connected Dominating Sets}}},
  booktitle = {2022 {{IEEE}} 47th {{Conference}} on {{Local Computer Networks}} ({{LCN}})},
  author = {Detzner, Peter and Gödeke, Jana and Bondorf, Steffen},
  date = {2022-09},
  pages = {447--454},
  issn = {0742-1303},
  abstract = {A Peer-to-Peer (P2P) network consists of a large number of nodes, where each node may have different capabilities and properties. Finding peers with specific capabilities and properties is challenging. Thus, we propose a practical solution to the problem of peer discovery, which is finding peers in the network according to a specified query. We contribute a peer discovery for an m-ary tree-structured P2P network by utilizing a connected dominating set (CDS), a technique that is typically used in unstructured networks. Our approach of constructing the CDS requires no additional communication cost, while nodes can insert, update and remove data within \textbackslash mathcalO(1). Each node of the CDS – a dominating set node – maintains only a limited number of nodes. We confirm the properties of our proposed solution by using the ns-3 discrete-event simulator. This includes, besides the degree of decentralism of the peer discovery, also the heterogeneity of peers.},
  eventtitle = {2022 {{IEEE}} 47th {{Conference}} on {{Local Computer Networks}} ({{LCN}})},
  keywords = {connected dominating set,Costs,overlay network,Overlay networks,peer discovery,peer-to-peer,Peer-to-peer computing,Runtime,tree-structured,Upper bound}
}

```

```plain
@inproceedings{detznerLowCostSearchTreeStructured2021,
  title = {Low-{{Cost Search}} in {{Tree-Structured P2P Overlays}}: {{The Null-Balance Benefit}}},
  shorttitle = {Low-{{Cost Search}} in {{Tree-Structured P2P Overlays}}},
  booktitle = {2021 {{IEEE}} 46th {{Conference}} on {{Local Computer Networks}} ({{LCN}})},
  author = {Detzner, Peter and Gödeke, Jana and Bondorf, Steffen},
  date = {2021-10},
  pages = {613--620},
  issn = {0742-1303},
  abstract = {Peer-to-Peer (P2P) networks are one way to create large-scale distributed systems. A single peer has only a limited view on other peers. Thus, efficient searching for other peers or their content is a key performance indicator. In this paper, we investigate the search efficiency in an m-ary tree-structured P2P overlay. While previous work aimed for balancing the maximum height of a node's sub-trees, we show that keeping the height balanced throughout the overall network – a property called null-balance – will increase search performance considerably. Simulations using the ns-3 discrete-event simulator show 50\% better performance w.r.t. required routing hops in these null-balanced trees. Therefore, we develop algorithms that keep a tree null-balanced if a node joins or departures. I.e., we prevent the need for restructuring. As we show, the cost of our efficient structure-preserving algorithms is easily set off by a relatively small number of search operations.},
  eventtitle = {2021 {{IEEE}} 46th {{Conference}} on {{Local Computer Networks}} ({{LCN}})},
  keywords = {balanced tree,Conferences,Key performance indicator,overlay networks,peer-to-peer,Peer-to-peer computing,Routing,Vegetation}
}

```