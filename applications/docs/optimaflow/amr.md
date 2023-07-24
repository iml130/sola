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

# Autonomous Mobile Robot (AMR)

AMRs are mobile robots that can execute tasks in a logistics context without human interaction.
For our purposes they are able to follow a predefined path and also execute special actions they are designed for.

To function in a [CPPS](../intralogistics.md) every AMR has a [logical](#logical) and a [physical](#physical) component.

<!-- using md_in_html extension. Note preview will not display this correctly -->
<figure markdown>
  ![**Figure 1:** Components and messages](../img/amr_logical_physical_overview.drawio.png)
  <figcaption markdown>**Figure 1:** Components and messages</figcaption>
</figure>
<!-- TODO Top Level is CPPS/CPPS Agents -->

## Logical

The AMR Logical Agent is a smart participant in the [CPPS](../intralogistics.md).
It is the AMR's interface to SOLA and will

- STN (Simple Temporal Network)
- aquire [Tasks](../glossary.md#t) using specified [Algorithms](optimization.md),
- report the status of Task execution to the corresponding [Material Flow Logical Agent](material_flow.md),and other participants
- aquire map/topology informations and send those to the [AMR Physical Asset](#physical)
- report the AMR's state and position to other agents (cpps) and
- Send [Transport Orders](../glossary.md#t) in form of a list of [Transport Order Steps](../glossary.md#t) to the AMR Physical Asset
<!-- - handle [Path Planning]() -->

As shown in **Figure 1**


<!-- ### AMR Logical Agent and Path Planning Module

Currently not available.

#### Centralized

#### Decentralized -->

### Communication with Material Flow Logical Agent

The communication interfaces depend on where the AMR Logical Agent is run.
It can be run either on the robot's hardware or on a server.

<!-- Is this part of Material Flow Logical Agent? -->

- SOLA connection
- AMR Logical Agent can run on robot Hardware using a wifi connection
- AMR Logical Agent can run on a server using a direct or hardwired connection
<!-- * TODO there's more -->

## Physical

The AMR Physical Asset is the representative of the real AMR.
It is the [AMR Logical Agent](#logical)'s interface to the real AMR and will

- simple
- only one Transport Order at a time, no queuing
- send the vehicles Description containing information about kinematics, general vehicle properties and special abilities to the AMR Logical Agent
- report AMR Status Updates consiting of the AMR's state (idle, working, error) and position to the AMR Logical Agent<br />
  will be offered other participants
- manage the execution of one [Transport Order](../glossary.md#t) at a time.
  Transport Orders are described in [Material Flow](./material_flow.md).
- report AMR Transport Order Updates, which are events that occurred related to the execution of Transport Order, to the AMR Logical Agent
- forward Transport Order Steps to execute to the real AMR as described in [Communication with real or simulated robot](#communication-with-real-or-simulated-robot)

As shown in **Figure 1**.

### Functionality

### Communication with AMR Logical Agent

- TCP connection
- Opened by AMR Logical Agent
- Connected to by AMR Physical Asset
- AMR Logical Agent can run on robot Hardware using a direct connection
- AMR Logical Agent can run on a server using a wifi connection
- AMR Logical Agent's address has to be configured in advance?

#### Handshake

1. Physical sends Description
2. Logical sends Map/Topology

#### Regular and periodic Communication

Physical sends

- AMR Status Update (AMR State and Position) on Position or AMR State change
- AMR Transport Order Update (Transport Order State and Position) on Transport Order State change

Logical sends

- Transport Order Steps if
    - a new Task is aquired and no Order is currently executed or
    - an Order is finished and another Order is queued for execution

### Communication with real or simulated robot

- The interface has to be implemented
- AMR Physical Asset will send Order Steps to the implementation and expect to receive a completion message
- TODO Asset Connector Picture
<!-- * error messages currently not available/planned -->

## Mobility

- calculated in AMR Logical Agent
- trapezoid model
- loading and unloading times
- AMR Mobility Helper (simulated mobility, estimations for planning)
- data from the real robot

