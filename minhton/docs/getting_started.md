# Getting Started

A high-level description, what can be used and what are the results 

## Code Example

- step-by-step (4 Nodes joining, Fanout, ...)
- What happens in the network (global view)
- Show Routing tables, after each join, figure, ...
- Leave, ...
- Show Routing tables, after each join, figure, ...

## Entity Search / Peer Discovery

Each node in the constructed peer-to-peer network may have different capabilities and properties.
To find peers with specific capabilities and properties, we use a practical solution to the problem of peer discovery, which is finding peers in the network according to a specified query.
We contribute a peer discovery for an m-ary tree-structured P2P network by utilizing a connected dominating set (CDS), a technique that is typically used in unstructured networks.
Our approach of constructing the CDS requires no additional communication cost, while nodes can insert, update and remove data within O(1).

A peer discovery query description is given according to boolean algebra consisting of logical *and*-, *or*- and *not*-operators.
Literals are comparisons with the key-value pairs (KVPs).
The peer discovery query result is expected to contain information about each peer whose KVPs satisfy the peer discovery query.

## Routing links

![Figure 1](./img/minhton_links.png "Routing links example")  
**Figure 1**: The node 3:4 in the example tree with fanout 2 maintains several types of links.

Each MINHTON node individually maintains links to selected nodes in the network based on the tree structure and its position in the tree as follows:

- a link to its parent node (unless it is the root node 0:0).
- links to up to *m* children nodes (*m* denotes also the fanout of the tree).
- a link to a left and right adjcent node based on an in-order traversal.
- links to selected same-level [neighbor nodes in routing tables (RT)](./programmers/concept.md#routingtable).
- links to all children of each same-level neighbor.
