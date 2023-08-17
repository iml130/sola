# Leave Procedure

If a node wants to leave the network it first has to call the `initiateSelfDeparture` method.
It nay need a successor, which it can find using `FIND_REPLACEMENT` messages.
`FIND_REPLACEMENT` messages are directed in an equivalent way as described for the [join algorithm](join.md#finding-the-correct-position).

When a node notices that it can leave its position without any violations, it sends a `REPLACEMENT_OFFER` message to the leaving node.
The leaving node accepts the offer by sending back a `REPLACEMENT_ACK` to the replacing node.
This message contains all routing information about the leaving node, so that the replacing node can reconstruct the routing information without any additional messages.

As soon as the `REPLACEMENT_ACK` message was sent, the leaving node has no reponsibilities anymore and has officially left the network.
Then the replacing node has the reponsibility to rebuild everything by firstly leaving its own position and then to take in the position of the left node.

<figure markdown>
  <a id="fig_leave_sequence"></a>
  ![Leave with Replacement Sequence](../img/leave_with_replacement_sequence.svg)
  <figcaption markdown>**Figure 1:** Leave with Replacement Sequence</figcaption>
</figure>

## Message sequence for a successful leave procedure (MINHTON Algorithm)

0. (Leaving Node *l* sends **Find Replacement (80)**) (If successor necessary)
1. (**Find Replacement (80)** eventually reaches successor *s*) (*s* can be *l* --> No successor necessary)
2. *s* sends **Sign Off Parent Request (82)** to its parent *ps*
3. *ps* sends **Lock Neighbor Request (84)** to its right neighbor *rn_ps*
4. *rn_ps* replies with **Lock Neighbor Response (86)** to *ps*
5. *ps* sends **Lock Neighbor Request (84)** to its left neighbor *ln_ps*
6. *ln_ps* replies with **Lock Neighbor Response (86)** to *ps*
7. *ps* sends **Remove Neighbor (60)** to its level neighbors
8. Level neighbors reply with **Remove Neighbor Ack (62)** to *ps*
9. *ps* sends **Sign Off Parent Answer (88)** to *s*
10. *s* sends **Remove Neighbor (60)** to its level neighbors
11. *s* sends **Remove and Update Neighbors (90)** or **Update Neighbors (64)** to its left adjacent *la_s*
12. *s* sends **Update Neighbors (64)** to its right adjacent *ra_s*
13. Level neighbors reply with **Remove Neighbor Ack (62)** to *s*
14. *la_s* sends **Remove Neighbor Ack (62)** to *s*
15. *ra_s* sends **Remove Neighbor Ack (62)** to *s*
16. *s* sends **Replacement Offer (92)** to *l*
17. *l* replies with **Replacement Ack (94)** to *s*
18. Successor *s* (who is now at the position of the node that left!) sends **Replacement Update (66)** to symmetrical neighbors
19. Symmetrical neighbors send **Remove Neighbor Ack (62)** to *s*
20. Parent of leaving node position *pl* sends **Replacement Update (66)** to level neighbors
21. Level neighbors reply with **Remove Neighbor Ack (62)** to *pl* or *s* (including *pl* replying to *s*)
22. *s* sends **Unlock Neighbor (96)** to *ps*
23. *ps* sends **Unlock Neighbor (96)** to *rn_ps* and *ln_ps*

## Terminology

*leaving/left node*: the node who leaves the network

*replacing node*: the node who is replacing the position of the leaving node

*replaced position*: the position where the leaving node was, which is now being replaced

*removed position*: the position where the replacing node was, but which is becoming empty now as the replacing node replaces the leaving node

*routing table neighbor parents*: the parents of our routing table neighbors. those neighbors have the current node as a routing table neighbor child, and need to be informed about the leaving too, although the current node does not have a direct connection to them. we can calculate the positions locally.

## Updating Network for Leave without Replacement

The leaving node must handle the updating of the network itself. For this the node has to send `REMOVE_NEIGHBOR` messages to each neighbor who knows the node (not its routing table neighbor children, but also its routing table neighbor parents).
To the routing table neighbor parents has to be sent via `SearchExact`, because the network information is not known.

After that the adjacent information has to be updated by linking the adjacent left and adjacent right of the leaving node together.

## Updating Network for Leave with Replacement

The replacing node must handle the updating of the network.
For this the nodes who know the replacing position and those who know the removing position are crucial.
Firstly we need to send `REMOVE_NEIGHBOR` messages to those nodes who know the removing position, but not the replacing position.
They only need to remove their entry of the removing position.

The nodes who know the leaving node and those who know both need to get `REPLACEMENT_UPDATE` messages.
They contain information about the removing and replacing position.
The nodes who know only the leaving node only need to update their Network Information with those of the replacing node.
Those who know both need to update their Network Information and at the same time remove the entry of the removing position from their routing information.

After that the adjacent information has to be updated.
For the adjacents of the removed position the adjacent left and adjacent right need to be linked together.
We do not need to update the adjacents of the replaced position separately, because this will already be done through the `REPLACEMENT_UPDATE` messages.

<figure markdown>
  <a id="fig_leave_example"></a>
  ![Leave with Replacement Example](../img/leave_example_01.svg)
  <figcaption markdown>**Figure 2:** Leave with Replacement Example</figcaption>
</figure>
