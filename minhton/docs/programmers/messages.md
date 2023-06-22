# Messages

Each message class is first and foremost made up of a header class. The header class contains a message type, target node, and sender node. The sender node is always the node that is sending exactly this message. The target node is always the node the message is directly sent to. In case the tree position of the target is not known, the network information must be set.

Additionally, the message header contains an event id and ref event id to identify events the message is initiating, respectively events the message is referring to. E.g., a join message is initiating a join procedure event and all originating update messages of that join procedure contain the join message's event id as their ref event id.

## Message Types -> **Do not maintain by hand! Link to DoxyGen Enum**

Todo: Link to Doxygen, remove all content (maintenance threshold is to high)

| Message Type                                              | Value |
|-----------------------------------------------------------|------:|
| **General**                                               |       |
| [INIT](#init)                                             | 0     |
| **Join Procedure**                                        |       |
| [JOIN](#join)                                             | 10    |
| [JOIN_ACCEPT](#join-accept)                               | 12    |
| [JOIN_ACCEPT_ACK](#join-accept-ack)                       | 14    |
| **Entity Search**                                         |       |
| [FIND_QUERY_REQUEST](#find-query-request)                 | 20    |
| [FIND_QUERY_ANSWER](#find-query-answer)                   | 22    |
| [ATTRIBUTE_INQUIRY_REQUEST](#attribute-inquiry-request)   | 24    |
| [ATTRIBUTE_INQUIRY_ANSWER](#attribute-inquiry-answer)     | 26    |
| [SUBSCRIPTION_ORDER](#subscription-order)                 | 28    |
| [SUBSCRIPTION_UPDATE](#subscription-update)               | 30    |
| **Search Exact**                                          |       |
| [SEARCH_EXACT](#search-exact)                             | 40    |
| [SEARCH_EXACT_FAILURE](#search-exact-failure)             | 41    |
| [EMPTY](#empty)                                           | 42    |
| **Bootstrap Algorithm**                                   |       |
| [BOOTSTRAP_DISCOVER](#bootstrap-discover)                 | 50    |
| [BOOTSTRAP_RESPONSE](#bootstrap-response)                 | 52    |
| **Updates & Response Algorithm**                          |       |
| [REMOVE_NEIGHBOR](#remove-neighbor)                       | 60    |
| [REMOVE_NEIGHBOR_ACK](#remove-neighbor-ack)               | 62    |
| [UPDATE_NEIGHBORS](#update-neighbors)                     | 64    |
| [REPLACEMENT_UPDATE](#replacement-update)                 | 66    |
| [GET_NEIGHBORS](#get-neighbors)                           | 70    |
| [INFORM_ABOUT_NEIGHBORS](#inform-about-neighbors)         | 72    |
| **Leave Procedure**                                       |       |
| [FIND_REPLACEMENT](#find-replacement)                     | 80    |
| [REPLACEMENT_NACK](#replacement-nack)                     | 81    |
| [SIGN_OFF_PARENT_REQUEST](#sign-off-parent-request)       | 82    |
| [LOCK_NEIGHBOR_REQUEST](#lock-neighbor-request)           | 84    |
| [LOCK_NEIGHBOR_RESPONSE](#lock-neighbor-response)         | 86    |
| [SIGN_OFF_PARENT_ANSWER](#sign-off-parent-answer)         | 88    |
| [REMOVE_AND_UPDATE_NEIGHBOR](#remove-and-update-neighbor) | 90    |
| [REPLACEMENT_OFFER](#replacement-offer)                   | 92    |
| [REPLACEMENT_ACK](#replacement-ack)                       | 94    |
| [UNLOCK_NEIGHBOR](#unlock-neighbor)                       | 96    |

These values can still change.

## Adding a new message type 

**Todo: can remain here, explain **

What needs to be done for adding a new message type:

- Create a new header file in `include/minhton/message` (Tip: Use join.h as a guideline)
    - Make sure to use a unique name for the ifndef include guide
    - Make sure to include `#include "minhton/message/message.h"` and `#include "minhton/message/serialize.h"`
    - Place a `SERIALIZE(...)` with required attributes under public visibility (Tip: Look at `include/minhton/algorithms/esearch/find_query.h` if the message type needs separate load and save functions)
    - Apart from the constructor for general usage which should have all parameters to create a valid message, a default constructor is required for serialization purposes.
    - Always required and the same are the `protected` fields that all message types must have.
- Create a corresponding new source file in `src/message`
- Include the message type header in `include/minhton/message/types_all.h` and extend the variant in alphabetical order
- Do the same in `include/minhton/message/se_types.h` for message types that can be used as a Search Exact query
- Include the source file in `src/message/CMakeLists.txt`
- Add an enum value in `include/minhton/message/types.h`
- Add the enum value to the correct interface header file in `include/minhton/algorithms/`
- Implement the message processing in the correct algorithm
- Add the test cases under `tests/`
- Adjust the state machine in `src/utils/fsm.cpp` and `include/minhton/utils/fsm.h`

## Using the message types

**TODO: Requires more explanaition**

1. Create a MinhtonMessageHeader with the sender and target NodeInfos as well as the ref event id.
2. Create the specific message with the header as the first parameter, followed by the parameters for the message type. (**Do not use the default constructor except for test cases!**)
3. Send the message.
4. The message is sent over the network using the NetworkFacade.
5. The message arrives at the target, where it is processed by the specified algorithm or handled by the `node.cpp`.

## Message Descriptions **Do not maintain by hand! Link to DoxyGen Enum**

### Init

**Usage**: Empty message.

**Content**:

None.

### Join

**Usage**: When a node wants to join the network, it sends a join message to one node in the network. If the fitting join position has been found, the target accepts the node as a new child. Otherwise, the target forwards the message with the set entering node further.

**Content**:

- *Entering Node*: The node that wants to enter the network. PhysicalNodeInfo must be set appropriately.

### Join Accept

**Usage**: A fitting join position has been found by forwarding the [join message](#join) appropriately. The target of the last join message sends a Join Accept message to the entering node (new child). This message contains all information the entering node needs about its new neighbors and the network's fanout.

**Content**:

- *Fanout*: Fanout of the entire network
- *Adjacent Left*: Adjacent left neighbor of the new child. Might be left empty if the new child has no adjacent left.
- *Adjacent Right*: Adjacent right neighbor of the new child. Might be left empty if the new child has no adjacent right.
- *RoutingTableNeighbors*: A vector of all existing routing table neighbors on the same level.

### Join Accept Ack

**Usage**: After receiving a [Join Accept](#join-accept) message, the new child must send a Join Accept Ack back to the new parent. It is needed to make sure the entering node still wants to join. Only after receiving this message, the parent node will send update messages through the network to inform the other neighbors about the joined node.

**Content**:

None

---

### Find Query Request

**Usage**: A node that wants to find data in the network can call the Entity Search Algorithm, which sends Find Query Requests to the requesting node's DSNs.

**Content**:

- *FindQuery*: Defines what kind of data the requesting node wants to aquire.
- *ForwardingDirection*: Can be either none, left, or right.
- *Interval*: A pair defining the start and end number of relevant nodes (both exclusive).

### Find Query Answer

**Usage**: A DSN sends a Find Query Answer back to the [requesting node](#find-query-request) with the data that was aquired.

**Content**:

- *NodesWithAttributes*: Mapping NodeInfos to Attributes. The attributes consist of multiple NoteDatas. NodeDatas have a string key and a corresponding value of varying types.

### Attribute Inquiry Request

**Usage**: A node that is not a DSN may receive a Attribute Inquiry Request during an ongoing Entity Search. An Attribute Inquiry Request messages always comes from a DSN, which can also be the requesting node itself.

**Content**:

- *MissingKeys*: The keys for which the node looks up its own local data. The corresponding value with a timestamp is added to the Attribute Inquiry Answer if it is present.
- *InquireAll*: If set to true, all existing local data is added to the answer message, ignoring the *MissingKeys* property.

### Attribute Inquiry Answer

**Usage**: The message that the node which received a [Attribute Inquiry Request](#attribute-inquiry-request) sends back.

**Content**:

- *InquiredNode*: NodeInfo of the current node that received the Attribute Inquiry Request
- *AttributeValuesAndTypes*: A map including the local data with timestamps and the information whether a value is static or dynamic.
- *RemovedAttributeKeys*: A vector of the keys for which their distributed data shall be removed.

### Subscription Order

**Usage**: A node can send a Subscription Order message to subscribe or unsubscribe itself from the data another node maintains for the specified keys.

**Content**:

- *Keys*: Multiple keys for which the node wants to change its subscription.
- *Subscribe*: True subscribes the node to the specified *Keys*, False unsubscribes the node.

### Subscription Update

**Usage**: If a node's local data is modified (inserted, updated or removed), it will send a subscription update the the subscribers of the key.

**Content**:

- *Key*: Key of the modified data
- *Value*: Value of the modified data

---

### Search Exact

**Usage**: A node wants to send a message to another node in the network, but does not know the physical address, only the tree position. The Search Exact message gets forwarded into the right direction until it reaches its destination.

**Content**:

- *DestinationNode*: The destination node of the message. (This is not the target node. The target is only the node the message gets forwarded to.)
- *Query*: Another internal message of any other message type which the inital sender wants to send to the destination node. Within the query message, the sender is the inital sender and the target the destination node.

### Search Exact Failure

**Usage**: If a node cannot further forward a [Search Exact](#search-exact) message, but is also not the target node, it sends back a Search Exact Failure to the requesting node.

**Content**:

- *DestinationNode*: The destination node of the message. (This is not the target node. The target is only the node the message gets forwarded to.)
- *Query*: Another internal message of any other message type which the inital sender wants to send to the destination node. Within the query message, the sender is the inital sender and the target the destination node.

### Empty

**Usage**: A helper message type for [Search Exact](#search-exact) tests. It has no functionality by itself. Not to be used outside of Search Exact tests.

**Content**:

None

---

### Bootstrap Discover

**Usage**: A node wants to join the network but does not have access to a node's network information to send the initial Join message to. Therefore a Bootstrap Discover gets send through a Multicast to discover a node's network information.

**Content**:

- *DiscoveryMessage*: A string.

### Bootstrap Response

**Usage**: After receiving a [Boostrap Discover](#bootstrap-discover) message, the node receiving the message might answer with a Bootstrap Response message, in case the node is in a connected state and a fitting join parent.

**Content**:

- *NodeToJoinTo*: The node information the sender of the Bootstrap Discover message may join to.

<!--### Swap Offer

**Usage**: A node (the initiator) wants to swap positions with another node (the acceptor). To initiate the swap process the initiator sends a Swap Offer message to the acceptor, containing all information the acceptor needs to replace the position of the initiator.

**Content**:

- *InitiatorNeighbors*: All neighbors the initiator knows about.

### Swap Ack

**Usage**: The acceptor is sending the Swap Ack back to the initiator as an answer to the [Swap Offer](#swap-offer) message. It contains all information the initiator needs to replace the position of the acceptor.

**Content**:

- *AcceptorNeighbors*: All neighbors the acceptor knows about.-->

---

### Remove Neighbor

**Usage**: A node is leaving the network. This is a notification to remove the given node from the routing information.

**Content**:

- *RemovedPositionNode*: The position of the node to remove

### Remove Neighbor Ack

**Usage**: Is sent as a reply to either a Replacement Update or Remove and Update Neighbor Message message to acknowledge the update of the routing information.

**Content**:

None.

### Update Neighbors

**Usage**: Informing a node about a new or updated neighbor. Used mainly in the join accept and leave procedure. The relationship of how to update the node has to be given, e.g. ADJACENT_LEFT.

**Content**:

- *NeighborsAndRelationships*: Vector of tuples of nodes and their neighbor relationship.

### Replacement Update

**Usage**: During the replacement process, the neighbor nodes of the leaving node / the replaced position need to get an update about the network information of the replaced position. Through the information in this message the neighbors can update the network information and eventually also remove the removed position from their information if they also have it as their neighbor.

**Content**:

- *RemovedPositionNode*: The node who is replacing and who's position will be empty afterwards.
- *ReplacedPositionNode*: The node who is leaving the network and who's position will be replaced by the replacing node afterwards.

### Get Neighbors

**Usage**: Currently only used in the join accept procedure in rare cases by the parent to get the correct adjacent neighbors for the child.
The sender is giving a vector of neighbor relationships which he wants to know about from the targets routing information. The answer is supposed to be a InformAboutNeighbors message.

**Content**:

- *NeighborRelationships*: A vector of all the neighbor relationships the sender wants to know about.

### Inform About Neighbors

**Usage**: This message is an answer to a GetNeighbors message. It contains the node informations about the requested relationships. The node sends the message back.

**Content**:

- *Neighbors*: Vector of the node informations.

---

### Find Replacement

**Usage**: A node wants to leave the network, but cannot leave the position directly because it would violate the trees balancing conditions. Another node that can leave the position has to be found to replace the position of the leaving node. For this, the Find Replacement Message gets forwarded (similar to Join messages) until a fitting node to replace is found.

**Content**:

- *NodeToReplace*: The node that wants to leave the network.

### Replacement Nack

**Usage**: A Replacement Nack message is sent back to the [node to replace](#find-replacement) in case of a failure during the leave process. A failure can occur when the chosen successor is already replacing enother node or a [Sign Off Parent Answer](#sign-off-parent-answer) informed the current node about an unsucessful sign off from the parent.

**Content**:

None.

### Sign Off Parent Request

**Usage**: The last node of the network, the chosen successor, sends a Sign Off Parent Request message to its parent. The parent later answers with a [Sign Off Parent Answer](#sign-off-parent-answer) to notify the successor if the leave is allowed to continue.

**Content**:

None.

### Lock Neighbor Request

**Usage**: The parent of a successor node sends a Lock Neighbor Request to its right and left neighbor. The receivers try to lock themselves for the leave procedure and respond with a Lock Neighbor Response message, indicating success or failure of locking themselves.

**Content**:

None.

### Lock Neighbor Response

**Usage**: The answer to a Lock Neighbor Request, received by the parent of a successor node.

**Content**:

- *Successful*: Indicates whether a node could successfully lock itself or not.

### Sign Off Parent Answer

**Usage**: The successor node waits for the Sign Off Parent Answer from its parent after sending a [Sign Off Parent Request](#sign-off-parent-request) to it.

**Content**:

- *Successful*: Indicates whether the leave process is allowed to continue.

### Remove And Update Neighbor

**Usage**: Used in the leave and response algorithms for updating the routing information.

**Content**:

- *ShouldAcknowledge*: Whether the receiving node needs to answer with an acknowledgement or not.
- *MessageRemoveNeighbor*: A [Remove Neighbor](#remove-neighbor) message with information about which neighbor needs to be removed from the routing information.
- *MessageUpdateNeighbors*: An [Update Neighbors](#update-neighbors) message with information about which neighbors needs to be updated from the routing information.

### Replacement Offer

**Usage**: A [Find Replacement](#find-replacement) Message reached a node that can replace the leaving node. The node sends a Replacement Offer to the leaving node to initiate the replacement process.

**Content**:

None

### Replacement Ack

**Usage**: The node that wants to leave the network receives a Replacement Offer from a node that is willing to replace its position. As a response, the leaving node sends all of its necessary information in the Replacement Ack message to the replacing node. After this message was sent, the node can finally leave the network.

**Content**:

- *Neighbors*: A vector of all neighbors the leaving node has information about

### Unlock Neighbor

**Usage**: Used for unlocking nodes after they were locked during a concurrent operation like leave.

**Content**:

None.
