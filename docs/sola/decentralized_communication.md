Using SOLA with the MINHTON and MINHCAST components as shown in Figure 1, SOLA works as a decentral organized communication middleware.
It omits the need for a dedicated communication instance as no node has a global view on all nodes.
Contrary to other communication middlewares such as [MQTT](https://mqtt.org/), [ROS 1](https://www.ros.org/), [OPC UA](https://opcfoundation.org/about/opc-technologies/opc-ua/), or [Data Distribution Service (DDS)](https://www.dds-foundation.org/what-is-dds-3/), each node in SOLA operates on a local limited view.

<figure markdown>
  <a></a>
    ![](../img/sola_example_minhton_minhcast.svg)
  <figcaption markdown>**Figure 1:** SOLA with MINHTON and MINHCAST components</figcaption>
</figure>

Figure 2 illustrates the logical view on a network with two SOLA peers and two topics, where both SOLA peers are subscribed to both topics.

``EventDisseminationMinhcast`` and ``ManagementOverlayMinhton`` are component implementations that are utilizing the MINHCAST (natter) and MINHTON component.
Detailed information on how these component implementations work can be found [here](component_implementations.md).


<figure markdown>
  <a></a>
    ![](../img/sola_example_minhton_minhcast1.svg)
  <figcaption markdown>**Figure 2:** Logical view of two SOLA peers with MINHCAST and MINHTON, both subscribed to two topics</figcaption>
</figure>
