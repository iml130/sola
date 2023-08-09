# Join Procedure

If a new node (entering node) wants to join the network, it sends a JOIN message to one known node in the network.

The node that receives a JOIN Message executes the processJoin method. A node who executes the processJoin method will either...

* forward the JOIN message to another node
* or accept the node as a child

## Finding the correct position

Figure 1 illustrates how the MINHTON algorithm consists of three parts in sequence to find the correct position for a joining node.
First, a null node (compare [glossary](../other/glossary.md)) has to be found.
When the first reached node is not a null node, we use the adjacent to reach a null node with a single hop.
Second, based on the routing table entries we determine whether we arrived on the last level or the one before it.
Additional hops may be necessary to finish this process.
Third and last, the last hops to the end of the tree occur.
For the join procedure, this means reaching the node which is the first one from top to bottom, then left to right, able to become the parent of a new node.

<figure markdown>
  <a id="fig_flow_diagram_join"></a>
  ![Flow diagram of the algorithm](../img/flow_diagram.svg#gh-light-mode-only)
  ![Flow diagram of the algorithm](../img/flow_diagram_dark.svg#gh-dark-mode-only)
  <figcaption markdown>**Figure 1:** Flow diagram of the algorithm responsible for finding the correct join position.</figcaption>
</figure>

## Message sequence for a successful join procedure

1. Entering node *n* sends **Message Join (10)** to a known node of the network
2. **Message Join (10)** eventually reaches correct null node that will become the parent *p*
3. *p* sends **Message Join Accept (12)** to *n* (target LogicalNodeInfo includes designated position of *n*)
4. *n* replies with **Message Join Accept Ack (14)** to *p*
5. *p* sends **Update Neighbors (64)** to right and left adjacents of *n*
6. *p* sends **Update Neighbors (64)** to its routing table neighbors
7. *p* sends **Update Neighbors (64)** to the routing table neighbors of *n*
8. Targets of steps 5 - 7 reply with **Remove Neighbor Ack (62)** to *p*
