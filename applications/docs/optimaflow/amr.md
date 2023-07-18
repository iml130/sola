<!--# TODO löschen: Material Flow

Hierarchy

- Material Flow
- Task
- Transport Order, Move Order, Action Order
- Transport Order Step, Move Order Step, Action Order Step

# Optimaflow

- application of sola and surrounding algorithms
- multiple [Material Flows](./material_flow.md)
- multiple AMRs
- multiple Loading Stations and Unloading Stations -->

<!-- Main Title, will not be shown in toc. Only titles marked ## and their subsections will show. -->

# Autonomous Mobile Robot (AMR)

[AMR](../glossary.md#a)s are robots that can execute tasks without human interaction.
For our purposes they are able to follow a predefined path or even navigate by themselves and also execute special actions they are designed for.

To function in a [CPPS](../intralogistics.md) every AMR has a [logical](#logical) and a [physical](#physical) component.

<!-- using md_in_html extension. Note preview will not display this correctly -->
<figure markdown>
  ![Components and messages](../img/amr_logical_physical_overview.drawio.png)
  <figcaption markdown>**Figure 1:** Components and messages</figcaption>
</figure>

<!-- caption/label can also be placed directly above the figure if pure markdown is used. This will place the caption above the figure. Note that preview will place it arbitrarily -->
<!-- **Figure 1:** Components and messages
![Components and messages](../img/amr_logical_physical_overview.drawio.png) -->

<!-- Third option would be to use a newline "  " between caption and figure -->
<!-- ![Components and messages](../img/amr_logical_physical_overview.drawio.png)<!--newline--><!-- Spaces "  " have to be added again -->
<!-- **Figure 1:** Components and messages  -->

## Logical

The AMR Logical Agent is a participant in the [CPPS](../intralogistics.md).
It is the AMR's interface to SOLA and will

- aquire map/topology informations and send those to the [AMR Physical Asset](#physical)
- aquire [Tasks](../glossary.md#t) using specified [Algorithms](optimization.md),
- report the status of Task execution to the corresponding [Material Flow Agent](material_flow.md),
- report the AMR's state and position to other agents and the [Management Overlay](../../../minthon/docs/) and
<!-- - handle [Path Planning]() -->
- Send TransportOrderSteps, MoveOrderSteps and ActionSteps to the AMR Physical Asset

As shown in **Figure 1**

## Physical

The AMR Physical Asset is the representative of the real AMR.
It is the [AMR Logical Agent](#logical)'s interface to the real AMR and will

- send information regarding kinematics, general vehicle properties and special abilities to the [AMR Logical Agent](#logical)
- report the AMR's state and position to the [AMR Logical Agent](#logical)
- manage the execution of Orders one at a time.
  Those Orders can be Transport Orders, Move Orders or Action Orders as described in [Material Flow](./material_flow.md).
- report events that occurred related to the execution of an Order to the AMR Logical Agent
- relay [Order Steps](../glossary.md#o) to execute to the real AMR

As shown in **Figure 1**.

## Managing Transport Orders

Each Transport Order will traverse the following states:<!-- TODO: what do they mean? -->

1. kCreated
2. kQueued
3. kStarted
4. kGoToPickupLocation
5. kReachedPickupLocation
6. kLoad
7. kLoaded
8. kGoToDeliveryLoacation
9. kReachedDeliveryLocation
10. kUnload
11. kUnloaded
12. kFinished
13. kError

A Transport Order is created in a Material Flow Logical Agent, will be queued in a AMR Logical Agent and will traverse all other states in a AMR Physical Asset as shown in **Figure 2**.
All state changes will be reported upwards to the Material Flow Logical Agent.

<figure markdown>
  ![States a Transport Order traverses](../img/amr_transport_order_flowchart.drawio.png)
  <figcaption markdown>**Figure 2:** States a Transport Order traverses, grouped by components that cause the transition.</figcaption>
</figure>

## Communication Interfaces

The communication interfaces depend on where the AMR Logical Agent is run.
It can be run either on the robot's hardware or on a server.

<!-- ### AMR Logical Agent and Path Planning Module

Currently not available.

#### Centralized

#### Decentralized -->

### AMR Logical Agent and Material Flow Logical Agent

- SOLA connection
- AMR Logical Agent can run on robot Hardware using a wifi connection
- AMR Logical Agent can run on a server using a direct or hardwired connection
<!-- * TODO there's more -->

### AMR Physical Asset and AMR Logical Agent

- TCP connection
- Opened by AMR Logical Agent
- Connected to by AMR Physical Asset
- AMR Logical Agent can run on robot Hardware using a direct connection
- AMR Logical Agent can run on a server using a wifi connection
- AMR Logical Agent's address has to be configured in advance?

### AMR Physical Asset and real or simulated robot

- The interface has to be implemented
- AMR Physical Asset will send Order Steps to the implementation and expect to receive a completion message
<!-- * error messages currently not available/planned -->
