## Database structure

<figure markdown>
  <a></a>
    ![DAISI SOLA logging](../img/logging_minhton.svg)
  <figcaption markdown>**Figure 1:** Database structure for logging MINTHON data with ns-3.</figcaption>
</figure>

- **MinhtonFindQuery**: Logs find queries
- **MinhtonFindQueryResult**: Logs only finished find queries
- **MinhtonNetworkInfo**: Associates the network information consisting of an IP address and a port with an application UUID for each node. Each node exists exactly once in this table.
- **MinhtonNode**: Provides a position UUID that combines the application UUID of a node with its tree position.
- **MinhtonNodeState**: When the state of a node changes an entry is added. This happens when 1) a node gets accepted into the network, 2) a node leaves the network (because the status changes to *left*), and 3) a node replaces another node (because the position changes).
- **MinhtonTraffic**: Each message that a node receives or sends will be logged in the MinhtonTraffic table.
- **MinhtonSearchContent**: Logs the content of Search Exact requests with their attribute name and type.
- **MinhtonSearchTest**: Only used for testing Search Exact with empty messages.
- **MinhtonRoutingInfo**: When the routing information about a neighbor of any node changes, a new entry is added. This happens when 1) a node gets a new neighbor, 2) the network information about neighbor gets updated, and 3) a neighbor leaves the network.

## Views

Since the tables partially contain values that require further context, we also provide some database views for easier understanding of the logged information.
The views are named after their originating tables:

- **viewMinhtonNodeState**
- **viewMinhtonTraffic**
- **viewMinhtonRoutingInfo**

## Further Information

### MinhtonTraffic

Most of the fields represent information about the content of the message.
Sender and target must always be set.
Depending on the message type the other fields might also be set.
In most cases only two additional fields for nodes are necessary.
Other information like a vector of nodes will be written into the Content field as a string.

### SearchTest

| Search Exact Status | Value | Interpretation                  |
|---------------------|-------|---------------------------------|
| Start               | 0     | Sender starts sending           |
| Hop                 | 1     | Intermediate step               |
| Success             | 2     | Target reached                  |
| Failure             | 3     | Target not found / non-existent |

When executing a search test, search exact messages will be sent from certain nodes to certain other nodes.
The search exact messages contain empty messages as a query without functionality.

The sender is the node who initializes the search exact messages and wants to send the message to the target / destination.
The hop nodes are nodes, who are not the sender and not the target, but intermediate steps along the way towards the destination.

The status of an entry can be either Start, Hop, Success or Failure.
A message is only successfully passed to the target if at exactly one entry with the Success status from sender to target exists.
The message passing has definitely failed if there is an entry with the Failure status.
Everything else should not happen and would be undefined behavior.
Entries with the Start status should not be count as messages passing through the network, because it shows the initial processing and forwarding of the message still within the sender node.

When a Failure status occurs this can either mean that the target node does not exist (which might happen), or that there is an error in the algorithm and the forwarding does not work correctly.

### RoutingInfo

To evaluate the routing information logged in the RoutingInfo table, it is necessary to calculate the latest valid entries at a given point in time, because the neighbor relationships are constantly changing.
For example while just building a network of 7 nodes with fanout 2, the adjacents of root change multiple times.
First 0:0 has the left adjacent 1:0, then later 2:1.
Here the left adjacent neighbor relationship will just be overwritten with the new adjacent left.

If a neighbor leaves the network and the connection is not being replaced, a new entry will be made with the fitting tree position (in case the tree position is unmistakable), but an invalid, i.e.
null, IP address and port.
If an adjacent neighbor will be removed, we don't know the tree position, because the tree position for an adjacent can change depending on the network.
Here the IP address and port will be invalid, as well as the tree position.
