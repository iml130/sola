# Architecture

## Networking structure

We introduced the intermediate facade-class ``natter::core::NetworkFacade`` to abstract away serializing and communicating with the low-level network library.
SolaNet is used as a low-level network library.

<center>
<img src="../img/network_structure.svg" alt="Examplary network" /><br />
Figure 1: Network structure of natter.
</center>
