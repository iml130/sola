# **TODO: Move to ns-3 Integration mostly** Logging

We are using a single sqlite database while running simulations with ns-3. Here the logging messages from each peer get put into one sqlite database from a given simulation run.
There is also the option to use a mysql database.

## General Side Notes

Further details about the database structure are explained in the [ns-3 Integration](../../ns-3/index.md) documentation part.

### IP Address

The ip address of a physical peer will never change in the simulation, whereas the tree position of a physical peer might change multiple times through e.g. leaves, replacements or other restructuring of the network. Therefore to determine the actual load of a peer it is necessary to compare the ip addresses and not the tree position. The used ip addresses are based on the IPv4 standard and will be logged as a string, e.g., "1.1.0.2". MINHTON is an application running on the physical peers, by default on port 2000.

### Timestamp

In the ns-3 simulation the timestamp is in the unit milliseconds. But it is easy to change the unit in the simulator to even higher resulutions.

## Tables

The database tables related to MINHTON are:

- **FindQuery**: Logs find queries
- **FindQueryResult**: Logs only finished find queries
- **MinhtonNetworkInfo**: Associates the network information consisting of an IP address and a port with an application uuid for each node. Each node exists exactly once in this table.
- **MinhtonNode**: Provides a position uuid that combines the application uuid of a node with its tree position.
- **MinhtonNodeState**: When the state of a node changes an entry is added. This happens when 1) a node gets accepted into the network, 2) a node leaves the network (because the status changes to *left*), and 3) a node replaces another node (because the position changes).
- **MinhtonTraffic**: Each message that a node receives or sends will be logged in the MinhtonTraffic table.
- **SearchContent**: Logs the content of Search Exact requests with their attribute name and type.
- **SearchTest**: Only used for testing Search Exact with empty messages.
- **RoutingInfo**: When the routing information about a neighbor of any node changes, a new entry is added. This happens when 1) a node gets a new neighbor, 2) the network information about neighbor gets updated, and 3) a neighbor leaves the network.

## Views

Since the tables partially contain values that require further context, we also provide some database views for easier understanding of the logged information. The views are named after their originating tables:

- **viewMinhtonNodeState**
- **viewMinhtonTraffic**
- **viewRoutingInfo**

## Further Information

### MinhtonTraffic

Mode means either that the message got sent or received.

| Mode | Interpretation |
|------|----------------|
| 0    | Receiving      |
| 1    | Sending        |

Every other field represents information about the content of the message. Sender and target must always be set. Depending on the message type the other fields might also be set. In most cases only two additional fields for nodes are necessary. Other information like a vector of nodes will be written into the Content field as a string.

### SearchTest

| Search Exact Status | Value | Interpretation                  |
|---------------------|-------|---------------------------------|
| Start               | 0     | Sender starts sending           |
| Hop                 | 1     | Intermediate step               |
| Success             | 2     | Target reached                  |
| Failure             | 3     | Target not found / non-existent |

When executing a search test, search exact messages will be sent from certain nodes to certain other nodes. The search exact messages contain empty messages as a query without functionality.

The sender is the node who initializes the search exact messages and wants to send the message to the target / destination. The hop nodes are nodes, who are not the sender and not the target, but intermediate steps along the way towards the destination.

The status of an entry can be either Start, Hop, Success or Failure. A message is only successfully passed to the target if at exactly one entry with the Success status from sender to target exists. The message passing has definitely failed if there is an entry with the Failure status. Everything else should not happen and would be undefined behavior. Entries with the Start status should not be count as messages passing through the network, because it shows the initial processing and forwarding of the message still within the sender node.

When a Failure status occurs this can either mean that the target node does not exist (which might happen), or that there is an error in the algorithm and the forwarding does not work correctly.

### RoutingInfo

To evaluate the routing information logged in the RoutingInfo table, is is necessary to calculate the latest valid entries at a given point in time, because the neighbor relationships are constantly changing. For example while just building a network of 7 nodes with fanout 2, the adjacents of root change multiple times. First 0:0 has the left adjacent 1:0, then later 2:1. Here the left adjacent neighbor relationship will just be overwritten with the new adjacent left.

If a neighbor leaves the network and the connection is not being replaced, a new entry will be made with the fitting tree position (in case the tree position is unmistakable), but an invalid, i.e. null, ip address and port. If an adjacent neighbor will be removed, we don't know the tree position, because the tree position for an adjacent can change depending on the network. Here the ip address and port will be invalid, as well as the tree position.
