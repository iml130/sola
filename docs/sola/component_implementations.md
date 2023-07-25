!!! danger "This page is work in progress!"

This page lists all components that are implemented for the use with SOLA, as well as specifics about using them.

For some component implementations it is required to have access to other components.
Hence currently the Publish-Subscribe component implementation has access to the service discovery component implementation.

For each component, a configuration object must be passed to the constructor of SOLA to configure the component implementation and the underlying component.
See [Using SOLA](usage.md) for more information about this.

![](../img/sola_component_impl_dependencies.svg)

## Service Discovery Component Implementations

The API for service discovery component implementations is described [here](https://iml130.github.io/sola/doxygen/classsola_1_1Storage.html).

### MINHTON
The [MINHTON](../management_overlay/introduction.md) overlay network is well suited for service discovery as it directly includes the required functionalities.
Hence the ``ManagementOverlayMinhton`` component implementation mostly forwards function calls only.
A description on how the service discovery works with MINHTON is described [here](TODO LINK TO MINHTON DOCS).

## Publish-Subscribe Component Implementations
The API for Publish-Subscribe component implementations is described [here](https://iml130.github.io/sola/doxygen/classsola_1_1EventDissemination.html).

### MINHCAST
The MINHCAST algorithm is integrated into the [natter](../natter_lib/introduction.md) framework.
It uses the MINHTON tree structure to send messages to all nodes in the tree by using unicasts.
Its inner workings are described [here]().

The component implementation ``EventDisseminationMinhcast`` creates separate MINHTON trees for every topic in the Pub-Sub environment.
As only nodes interested in a particular topic are part of a topic tree, only they are receiving messages published for this topic.
The connection between nodes in the MINHTON topic trees are forwarded to MINHCAST together with the corresponding topic and are used for publishing the message.
On ``subscribeTopic(..)``, the component implementation must also pass its own node info (MINHTON level and number) for its topic tree node. 
The connection information (IP and port) of node in the topic tree must be known to join into an existing MINHTON topic tree (see [MINHTONs Join Procedure](../management_overlay/algorithms/join.md)).
The component implementation uses the Service Discovery to discover these information.
By using the service discovery like a key-value database, the component implementation searches for a key with the name of the topic.

If no result is found, it is assumed that this topic does not exist yet, as well as no MINHTON topic tree.
A new MINHTON node is created as a root for a new topic tree.
Its connection information is inserted into the service discovery system (``key: "<TopicName>"`` and ``value="<IP>:<Port>"``).

If a result (node to join on) is found, a new MINHTON node is created and the result is used to join the topic tree.
The information of this new node is **not** inserted into the service discovery system.

!!! warning

    Currently the topic cannot be unsubscribed with MINHCAST.
    <!-- TODO See issue  -->


### Central Publish-Subscribe
!!! danger "This component implementation is currently work in progress"

<!-- Client Server -->

## Exemplary logical representation of SOLA with MINHTON and MINHCAST

![](../img/sola_example_minhton_minhcast.svg)

We mainly use SOLA together with the MINHTON and MINHCAST components.
The following figure illustrates the logical view on a network with two SOLA peers and two topics, where both SOLA peers are subscribed to both topics.

![](../img/sola_example_minhton_minhcast1.svg)
