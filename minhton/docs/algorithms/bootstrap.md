# Bootstrap

We use bootstrap to discover other nodes on the network and thereafter join on them.

A new node wanting to join the network but didn't know any other node (IP and port) will send a discovery message. All so called "bootstrap nodes" will answer to this request. The new node will then continue to join the network using the node of the first discovery-answer it receives.

Currently three modes can be used which defines which nodes will be "bootstrap nodes".

- **All nodes**: All nodes will become bootstrap nodes. This will likely cause much network traffic! (k_MULTICAST_STRATEGY = 0)
- **Prio-Nodes**: All [prio-nodes](../other/glossary.md) become bootstrap nodes (k_MULTICAST_STRATEGY = 1)
- **Nodes with free childs**: Nodes with place for new childs become bootstrap nodes (k_MULTICAST_STRATEGY = 2)
