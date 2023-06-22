# MINHTON Glossary

## General

- *Maintenance*: Maintenance is a process of applying changes to the tree structure while retaining the optimized balance criterion.
    - *Join*: A process in which the overlay network is extended by a node, which is not a part of the network before the process starts.
    - *Leave*: A process in which the overlay network is reduced by a node. The node is currently part of the network, but will leave the network during the process.
<!--- Entity Search: ...
- Swapping: ...-->

## Node / Tree related

- *Level*: The level is a value for the vertical position of a node within the tree stucture.
- *Number*: The number is a value for the horizontal position of a node within a level.
- *Fanout m*: The fanout *m* is the maximum number of children any node can have. For example in a tree with fanout 3 a node may have up to 3 children.
- *N*: *N* is the total number of nodes in the network.
- *Notation*: The notation to identify a node is Level:Number, e.g. 2:4.
- *Root node*: This node is 0:0 and must always exist, otherwise there is no starting point for the overlay structure.
- *Last node*: This is the rightmost node on the lowest level. E.g. in a network consisting of the nodes 0:0, 1:0, 1:1, 1:2 and 1:3, this is 1:3.
- *Routing Table (RT)*: Each node has a routing table which contains (the positon and network address of) other nodes on the same level. The other nodes are selected based on a formula from the BATON / BATON\* paper. Nodes in close proxmity are more likely to be included.
- *TreeMapper value*: The TreeMapper assigns a floating-point value to each node. The value resembles the global horizontal value, defaulting to 100.0 for the root node, 50 for the node 1:0 and 150 for the node 1:1 with fanout 2.
- *LogicalNodeInfo*: Logical information about a node like level and number.

## Maintenance Algorithm

- *Null node*: A null node has less children than the fanout allows. Following the balacing criteria, a null node can only exist on the last level or the one before it.
- *Accept as a child*: An existing node in the network can accept a new node as its child, when the balance is kept. This means the new node inhabits the position of one of *m* children on the level below of the existing node.
- *Leaving Node*: Node that wants to leave the network.
- *Successor Node*: Node that is found with ``FindReplacement`` to be the last node in the network (rightmost on lowest level). This node will be used to replace the *Leaving Node*.

The two possible leave cases:

- Node that wants to leave is last node of the network (either directly knows that it is the last node or sends a FindReplacement and received it back)
- Node that wants to leave needs a replacement

## Entity Search Algorithm

- *Dominating Set Node (DSN)*: DSNs are a subset of nodes establishing a Dominating Set (DS) on each level. Using only DSNs, every node in the network can be reached by just another hop from the right DSN at maximum.
