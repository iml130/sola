When using SOLA with the MINHTON and MINHCAST components, SOLA is a decentral organized communication middleware that omits the need for a dedicated communication instance.
Contrary to other communication middlewares such as [MQTT](https://mqtt.org/), [ROS 1](https://www.ros.org/), [OPC UA](https://opcfoundation.org/about/opc-technologies/opc-ua/), or [Data Distribution Service (DDS)](https://www.dds-foundation.org/what-is-dds-3/), each node in SOLA operates on a local limited view where *O(n) while n << N*.
The local view is of each peer maintains selected links to a subset of all peers in its routing table (RT).

In the following Figure 1, each peer within SOLA is represented by a single entity. 
Furthermore, the nodes are required to collaborate in order to appear to its user as a single coherent system
<figure markdown>
![SOLA](../img/sola_app.svg)
<figcaption markdown>**Figure 1**: SOLA appears to it users as a single coherent system. As each node in SOLA has a limited view, a collaboration among all nodes is required.</figcaption>
</figure>

Entities, such as automated guided vehicles (AGVs), autonomous mobile robots (AMRs) or even virtual entities such as material flows or a production orders, can join and become part of SOLA.
As SOLA uses Pub-Sub and Service Discovery, this enables a loose coupling during operational runtime.

## Why decentral organized communication?

Industry 4.0 (I4.0) is the current ongoing fourth industrial revolution.
Key-pillars of the I4.0 are interconnectivity, information transparency, decentralized decision-making, and technical assistance.
Interconnectivity means that a wide variety of machines, sensors and devices can communicate and interact with each other.
Standardized interfaces are used for this purpose.
Through the aforementioned interconnectivity, the real world is unified with the virtual world.
Exchanged information can be put into context.
Context-related information is the basis for further decision-making.
Decentralized decision-making is based on networking and information transparency.
Local information is linked with global information.
Participants act on this aggregated information.
Technical assistance ensures that the increasingly complex processes remain manageable.
This support is particularly important for humans, since their role as machine operators is evolving into that of problem solvers.

## Example: SOLA with MINHTON and MINHCAST

![](../img/sola_example_minhton_minhcast.svg)

We mainly use SOLA together with the MINHTON and MINHCAST components.
The following figure illustrates the logical view on a network with two SOLA peers and two topics, where both SOLA peers are subscribed to both topics.

![](../img/sola_example_minhton_minhcast1.svg)
