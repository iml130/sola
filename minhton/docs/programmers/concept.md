# Concept

## NodeInfo

<figure markdown>
  <a id="fig1"></a>
  ![Class diagram for NodeInfo and related classes](../img/nodeinfo_classes.svg)
  <figcaption markdown>**Figure 1:** A NodeInfo contains a NodeStatus, a LogicalNodeInfo as well as a PhysicalNodeInfo.</figcaption>
</figure>

The [NodeInfo](https://iml130.github.io/sola/doxygen/classminhton_1_1NodeInfo.html) class can be used to represent all information about a neighbor node or a node itself.
It contains logical information about e.g. the position in the tree and the fanout, and information about the physical layer like IP address and port.
The logical information is stored in the [LogicalNodeInfo](https://iml130.github.io/sola/doxygen/classminhton_1_1LogicalNodeInfo.html) class and physical layer information in the [PhysicalNodeInfo](https://iml130.github.io/sola/doxygen/classminhton_1_1PhysicalNodeInfo.html) class, as shown in [Figure 1](#fig1).

The comparison operators are only comparing the LogicalNodeInfo objects and use directly the corresponding LogicalNodeInfo comparison operators.

### LogicalNodeInfo

This class contains the logical information about a node in the network, most importantly the level, number, and fanout.
Through these tree values the position of a node in the tree is given.
There is an additional helper attribute to check if the object is initialized, which says whether this tree position exists or not.

Only positive numbers (uint32_t) can be set for level and number.
To represent a non-existing LogicalNodeInfo object use the default constructor `LogicalNodeInfo()`.
There the attribute `initialized` is set to `false`.
This can only be changed again by replacing the entire position with a `setPosition` call.

There are different ways to construct an object of this class.
Most of the time you should only use the constructor `LogicalNodeInfo(level, number, fanout)`.

The constructor `LogicalNodeInfo(level, number)` and `LogicalNodeInfo(fanout)` must not be used for creating nodes.
They are only there for searching and testing purposes as well as similar use cases in algorithms.

To change the position of an object after construction, use `setPosition(level, number)` or `setPosition(LogicalNodeInfo)`, because level and number often must be set at the same time and not after another to avoid the setting of invalid tree positions.

The comparison operators are explicitly using the [tree mapper](../algorithms/treemapper.md) function.
This means that if for the LogicalNodeInfo objects x and y `x < y` is true, x is to the left of y and y is to the right of x.

To compare different properties like the depth within the tree there are different methods like `isSameLevel()` or `isDeeperThan()`.

### PhysicalNodeInfo

This class contains information about the physical layer, namely the IP address and port.
At the moment only IPv4 addresses are supported.

The comparison operators calculate a value out of the IP address and node which do not have a direct meaning.
But they are unique values so that they can be used for sorting etc.

## RoutingInformation

In the routing information, all information to each known neighbor is stored as NodeInfo objects.
This includes the parent, children, adjacent left and right, and left and right routing tables.

### Neighbor Relation Properties

There are some properties to keep in mind before we get into the details of RoutingInformation and RoutingTable.

#### Symmetrical vs. Asymmetrical

**Symmetrical Neighbors**: If node A has node B as a neighbor, node B also has node A as a neighbor.

**Asymmetrical Neighbors**: If node A has node B as a neighbor, but B does not have node A as a neighbor.

Symmetrical Neighbors are parent, children, routing table neighbors, and adjacent left and adjacent right:

* if A is the parent of B, B is the child of A
* if A is a routing table neighbor of B with the distance x, B is also a routing table neighbor of A with the distance x, because the distance will stay the same
* if A is the adjacent left of B, B is the adjacent right of A

Asymmetrical Neighbors are the routing table neighbor children.
A node knows about the children of a routing table neighbor, in most cases that child will not know the node.

These properties are important to keep in mind when sending messages to update other nodes in the network about a join-accept, leave, leave-replacement, etc.

#### Neighbors with static vs. dynamic **Positions**

**Static Positions**: The tree position of that potential neighbor will always stay the same, independent of the network arrangement.

**Dynamic Positions**: The tree position might change, depending on the network arrangement.

Neighbors with dynamic positions are the adjacent left and adjacent right.
In a network of fanout 2 with 3 nodes in total, 0:0 will have 1:0 and 1:1 as its adjacent left and right.
But when the network grows bigger to 7 nodes in total, 0:0's adjacent will become 2:1 and 2:2.
Therefore the adjacent left and right can always change.
The only property is sure, that the adjacent left needs to be on the left of the node, and the adjacent right on the right.

Every other neighbor has a static position.

### RoutingTable

In a routing table a MINHTON node stores neighbors who are on the same level and have a certain horizontal distance to the node.
The distance can be calculated with the formula from the original BATON* paper [[1]](#references), e.g.:

* with fanout 2 nodes have neighbors with the distance 1, 2, 4, 8, 16...
* with fanout 3 nodes have neighbors with the distance 1, 2, 3, 6, 9, 18...

The formula goes as follows: { d \* m^i | 1<= d <= m-1 ; i >= 0 }.

Routing table information is stored as two vectors in `RoutingInformation`, `routing_table_neighbors_` and `routing_table_neighbor_children_`.
There the nodes are always stored from left to right.

If we look at node 3:3 with fanout 2, the node will have the neighbors 3:1 and 3:2 to its left, and 3:4, 2:5, and 3:7 to its right.
So the `routing_table_neighbors_` contains those neighbors in the order 3:1, 3:2, ..., 3:7.

Additionally, we are storing the children of those routing table neighbors.
This vector always has the size of fanout \* (size of `routing_table_neighbors_`).
The first child of the routing table neighbor at index i, has the position (i \* fanout) in the routing table neighbor children vector.
The last child is stored at position (i \* fanout + fanout-1).

Because NodeInfo uses the comparison operators of LogicalNodeInfo, we can use functions like std::find to efficiently execute operations on those vectors without needing to iterate over them with a for-loop.

<figure markdown>
  <a id="fig2"></a>
  ![Routing table Example](../img/example_routing_table.svg)
  <figcaption markdown>**Figure 2:** The node 3:3 in the example tree with fanout 2 maintains bidirectional links to the blue highlighted nodes on the same level in its routing table.
  One-directional links exist to the nodes on the level below highlighted in light blue.</figcaption>
</figure>

### Using getters and setters (and resetters)

When using a setter method, the tree position of the NodeInfo objects needs to have the right position.
When setting a neighbor with a static position, the set NodeInfo objects must have the same position.
The parent position of 1:0 e.g. may never deviate from 0:0.
When setting a neighbor with a dynamic position (adjacents) the position must be on the correct side of the node, e.g., with fanout 2 0:0 may not have an adjacent left neighbor like 1:1 because 1:1 is on the right side of 0:0.
The adjacent left neighbor of 0:0 must always be on its left side, e.g., 1:0, 2:1, or 3:3.

Also when using setters the NodeInfo object must always be initialized (must exist).
If a NodeInfo object is not initialized, either PhysicalNodeInfo or LogicalNodeInfo is not initialized.
LogicalNodeInfo always needs to be initialized due to the correct tree position.
PhysicalNodeInfo also must be initialized by having a correct IP address and port.

When you want to remove a neighbor (e.g. when the neighbor leaves) you cannot use setters for this purpose.
You must explicitly use the reset methods to make the neighbor non-existent, to not accidentally remove a neighbor somewhere else.
A reset method for the parent node is not supported, because it would logically not make sense to remove the parent node.

## References

[1] H. V. Jagadish, B. C. Ooi, K.-L. Tan, Q. H. Vu, and R. Zhang, „Speeding up search in peer-to-peer networks with a multi-way tree structure“, in Proceedings of the 2006 ACM SIGMOD international conference on Management of data  - SIGMOD ’06, Chicago, IL, USA, 2006, S. 1. doi: 10.1145/1142473.1142475.
