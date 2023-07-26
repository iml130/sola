# Architecture

***natter*** provides an interface containing broadcast-related methods which must be implemended by the broadcast algorithms (see [Using natter](../using.md#implement-a-broadcast-algorithm)).

Further, ***natter*** provides logging, serialization and networking functionalities that ***can*** be used by the broadcast algorithms.
For networking, the intermediate facade-class ``NetworkFacade`` abstracts away serializing and communicating with the low-level network library SolaNet.

<figure markdown>
  ![natter architecture](../img/architecture.svg)
  <figcaption markdown>**Figure 1:** Architecture of natter</figcaption>
</figure>
