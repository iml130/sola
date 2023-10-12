# Introduction

## Getting started

To run OptiMaFlow please download and build the repository as described [here](../getting_started.md).
Afterwards, follow the instructions for [OptiMaFlow](../daisi/using.md) in the ns-3 Integration tab.

## Intralogistics

In recent years, the domain of intralogistics has experienced rapid advancements in technology, driven by the fourth industrial revolution, also known as Industry 4.0 [[1]](#references). 
Industry 4.0 includes the convergence of physical hardware with cyber-applications, creating a new possible paradigm for warehouse logistics [[2]](#references). 
In a *cyber space*, entities have representations as virtual objects enabling
self-organization, synergy, and emergence. 
This paradigm is built on design principles such as interconnectivity, information transparency, and decentralized decision-making [[1]](#references). 
Autonomous mobile robots (AMRs) have emerged as a key component in this shift towards decentralized decision-making, enabling them to operate autonomously and adapt to changing conditions without centralized control.

AMRs utilize a decentralized decision-making process for collision-free navigation, making them highly flexible [[3]](#references). 
This flexibility is crucial in addressing the growing need for adaptable and reconfigurable warehouse logistics solutions, particularly in dynamic production lines and in response to disruptions in the supply chain. 
AMRs, with their ability to independently make decisions and navigate in a decentralized manner, offer significant advantages in meeting these evolving demands.


## What is OptiMaFlow?

A material flow in OptiMaFlow is a declarative description of the transport of materials.
OptiMaFlow, short for **Opti**mization of **Ma**terial **Flow**s, is responsible for intelligently assigning tasks of a [Material Flow](./participants//material_flow.md) to [Autonomous mobile robots (AMRs)](./participants/amr.md).
Managing the task assignment of an AMR presents challenges, as these problems are typically NP-hard. 
The complexity makes finding efficient solutions difficult and computationally expensive.

In OptiMaFlow, the cyber space consists of Logical Agents [[4]](#references). 
Each AMR has its own Logical Agent responsible for managing its physical assets. 
Additionally, there are Material Flow Logical Agents, which have no representation in the physical environment. 
A Material Flow Logical Agent exists purely in the cyber space and is responsible for securing the successful allocation and execution of a set of tasks.
All together, these Logical Agent communicate with each other to complete the material flow in a self-organized manner.
To address the task assignment challenge, we have implemented both centralized and decentralized algorithms within OptiMaFlow.


## References

[1] M. Hermann, T. Pentek, and B. Otto, “Design Principles for Industrie 4.0 Scenarios”, in 2016 49th Hawaii International Conference on System Sciences (HICSS), pp. 3928–3937, Jan. 2016.

[2] L. Monostori, “Cyber-physical Production Systems: Roots, Expectations and R&D Challenges”, Procedia CIRP, vol. 17, pp. 9–13, Jan. 2014.

[3] G. Fragapane, R. de Koster, F. Sgarbossa, and J. O. Strandhagen, “Planning and control of autonomous mobile robots for intralogistics: Literature review and research agenda,” European Journal of Operational Research, vol. 294, no. 2, pp. 405–426, Oct. 2021.

[4] D. Lünsch, P. Detzner, A. Ebner, and S. Kerner, “SWAP-IT: A Scalable and Lightweight Industry 4.0 Architecture for Cyber-Physical Production Systems,” in 2022 IEEE 18th International Conference on Automation Science and Engineering (CASE), Aug. 2022, pp. 312–318. 
