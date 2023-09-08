# Glossary MINHTON

## A

**Accept as a child**:<br />
An existing node in the network can accept a new node as its child when the balance is kept.
This means the new node inhabits the position of one of *m* children on the level below of the existing node.

## D

**Dominating set node (DSN)**:<br />
DSNs are a subset of nodes establishing a dominating set (DS) on each level.
Using only DSNs, every node in the network can be reached by just another hop from the right DSN at maximum.

## F

**Fanout m**:<br />
The fanout *m* is the maximum number of children any node can have.
For example, in a tree with fanout 3 a node may have up to 3 children.

## J

**Join**:<br />
A process in which the overlay network is extended by a node, which is not a part of the network before the process starts.

## L

**Last node**:<br />
This is the rightmost node on the lowest level.
E.g. in a network consisting of the nodes 0:0, 1:0, 1:1, 1:2, and 1:3, this is 1:3.

**Leave**:<br />
A process in which the overlay network is reduced by a node.
The node is currently part of the network but will leave the network during the process.

**Leaving node**:<br />
A node that wants to leave the network.

**Level**:<br />
The level is a value for the vertical position of a node within the tree structure.

**LogicalNodeInfo**:<br />
Logical information about a node like level and number.

## M

**Maintenance**:<br />
Maintenance is a process of applying changes to the tree structure while retaining the optimized balance criterion, either by a [Join](#j) or [Leave](#l) operation.

## N

**N**:<br />
*N* is the total number of nodes in the network.

**Notation**:<br />
The notation to identify a node is Level:Number, e.g. 2:4.

**Null node**:<br />
A null node has fewer children than the fanout allows.
Following the balancing criteria, a null node can only exist on the last level or the one before it.

**Number**:<br />
The number is a value for the horizontal position of a node within a level.

## P

**PhysicalNodeInfo**:<br />
Physical information about a node like networking information.

## R

**Removed position**:<br />
The position where the replacing node was, but which is becoming empty when the replacing node replaces the leaving node.

**Replaced position**:<br />
The position where the leaving node was, which is being replaced.

**Replacing node**:<br />
The node who is replacing the position of the leaving node.

**Root node**:<br />
This node is 0:0 and must always exist, otherwise, there is no starting point for the overlay structure.

**Routing table (RT)**:<br />
Each node has a routing table that contains (the position and network address of) other nodes on the same level.
The other nodes are selected based on a formula from the BATON / BATON\* paper.
Nodes nearby are more likely to be included.

**Routing table (RT) neighbor parents**:<br />
The parents of our routing table neighbors.
Those neighbors have the current node as a routing table neighbor child, and need to be informed about it joining / leaving too, although the current node does not have a direct connection to them.
We can calculate the positions locally.

## S

**Successor node**:<br />
A node that is found with ``FindReplacement`` is proposed to be the last node in the network (rightmost on the lowest level).
This node will be used to replace the [*leaving node*](#l).

## T

**TreeMapper value**:<br />
The TreeMapper assigns a floating-point value to each node.
The value resembles the global horizontal value, defaulting to 100.0 for the root node, 50 for the node 1:0, and 150 for the node 1:1 with fanout 2.
