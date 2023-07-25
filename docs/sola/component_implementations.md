This page lists all components that are implemented for the use with SOLA, as well as specifics about using them.

For some component implementations it is required to have access to other components.
Hence the Publish-Subscribe component implementation has access to the service discovery component implementation (see Figure 1).

For each component, a configuration object must be passed to the constructor of SOLA to configure the component implementation and the underlying component.
See [Using SOLA](usage.md) for more information about this.

<figure markdown>
  <a></a>
    ![](../img/sola_component_impl_dependencies.svg)
  <figcaption markdown>**Figure 1:** Internal SOLA dependency between the Pub-Sub and Service Discovery component implementation</figcaption>
</figure>

## Service Discovery Component Implementations

The API for service discovery component implementations is documented [here](https://iml130.github.io/sola/doxygen/classsola_1_1Storage.html).

### MINHTON
The [MINHTON](../management_overlay/introduction.md) overlay network is well suited for Service Discovery as it directly includes the required functionalities.
Hence the ``ManagementOverlayMinhton`` component implementation mostly forwards function calls only.
A description on how the Service Discovery with MINHTON works will be published soon.

## Publish-Subscribe Component Implementations
The API for Publish-Subscribe component implementations is documented [here](https://iml130.github.io/sola/doxygen/classsola_1_1EventDissemination.html).

### MINHCAST
The MINHCAST algorithm is integrated into the [natter](../natter_lib/introduction.md) framework.
It uses the MINHTON structure to send messages to all nodes in the tree by using unicasts.
Its inner workings are described [here](../natter_lib/programmers/minhcast.md).

The component implementation ``EventDisseminationMinhcast`` creates separate MINHTON networks (separate trees) for every topic in the Pub-Sub environment.
As only nodes interested in a particular topic are part of a topic tree, only they are receiving messages published for this topic.
The connection between nodes in the MINHTON topic trees are forwarded to MINHCAST together with the corresponding topic and are used for publishing the message.
On ``subscribeTopic(..)``, the component implementation must also pass its own node info (MINHTON level and number) for its topic tree node. 
The connection information (IP and port) of node in the topic tree must be known to join into an existing MINHTON topic tree (see [MINHTONs Join Procedure](../management_overlay/algorithms/join.md)).
The component implementation uses the Service Discovery to discover these information.
By using the service discovery like a key-value database, the component implementation searches for a key with the name of the topic.

If no result is found, it is assumed that this topic does not exist yet, as well as no MINHTON topic tree.
A new MINHTON node is created as a root node for the new topic tree.
Its connection information is inserted into the service discovery system (``key: "<TopicName>"`` and ``value="<IP>:<Port>"``).

If a result (node to join onto) is found, a new MINHTON node is created and the result is used to join the topic tree.
The information of this new node is **not** inserted into the service discovery system.

!!! warning

    Currently the topic cannot be unsubscribed with MINHCAST.
    <!-- TODO See issue  -->


### Central Publish-Subscribe
!!! danger "This component implementation is currently work in progress"

<!-- Client Server -->
