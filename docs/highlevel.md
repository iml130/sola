**TODO: Explanaition of the horizontal bullet points** 

<!--
TODO: Overview DAISI + SOLA + remaining components
 -->
- **SOLA (Standalone)** is the component that initializes and uses MINHTON as well as natter. SOLA can be used as the middleware for all communication purposes.
- minhton: MINimal Height Tree Overlay Network (**[MINHTON](minhton_lib/index.md)**) is a peer-to-peer management overlay which organizes peers in an optimized tree structure. 
- natter

Together, they form the decentralized communication middleware SOLA.

- **D**ecentralized tr**A**nsport **I**ntralogistic **SI**mulation (**DAISI**) provides a configuration for the network simulator ns-3 and includes the applications
    - self-organized material flow (**[SOMF](daisi_lib/applications/cpps/index.md)**), and
    - **[Path Planning](daisi_lib/applications/path-planning/index.md)**.
- **SOLANET** is a basic asynchronous UDP networking library for establishing connections between the peers.


We are using **[ns-3](ns-3/index.md)**, a discrete-event network simulator for Internet systems as the simulation environment for SOLA.
ns-3 is targeting primarily research and educational purpose.
However, we are using it for the development of an Industry 4.0 application and communication technologies.
The only other dependencies are the unit testing framework **Catch2** and the serialization library **cereal**.

The entry point for running the simulation is located in [daisi/src/main/daisi_main.cpp](https://github.com/iml130/sola/blob/main/daisi/src/main/daisi_main.cpp).

