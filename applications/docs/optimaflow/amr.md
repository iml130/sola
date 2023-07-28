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

<figure markdown>
  ![**Figure 1:** Components and messages](../img/amr_logical_physical_overview.png)
  <figcaption markdown>**Figure 1:** Components and messages</figcaption>
</figure>

## Logical

The AMR Logical Agent is a smart participant in the [CPPS](../intralogistics.md).
It is the AMR's interface to SOLA and will

- STN (Simple Temporal Network)
- aquire [Tasks](../glossary.md#t) using specified [Algorithms](optimization.md),
- report the status of Task execution to the corresponding [Material Flow Logical Agent](material_flow.md),and other participants
- aquire map/topology informations and send those to the [AMR Physical Asset](#physical)
- report the AMR's state and position to other agents (cpps) and
- Send [Transport Orders](../glossary.md#t) in form of a list of [Transport Order Steps](../glossary.md#t) to the AMR Physical Asset

As shown in **Figure 1**

### Communication with Material Flow Logical Agent

The communication interfaces depend on where the AMR Logical Agent is run.
It can be run either on the robot's hardware or on a server.

<!-- Is this part of Material Flow Logical Agent? -->

- SOLA connection
- AMR Logical Agent can run on robot Hardware using a wifi connection
- AMR Logical Agent can run on a server using a direct or hardwired connection
<!-- * TODO there's more -->

## Physical

The AMR Physical Asset is the [AMR Logical Agent](#logical)'s interface to the real AMR.
It will receive Orders from and send informations to the AMR Logical Agent through the TCP Connection.
The AMR Physical Asset can handle one Order at a time.
The Order will be represented by a series of [Functionalities](#functionality).

To manage Orders the AMR Physical Asset uses a finite state machine (FSM).
The FSM relays the Order Steps through the Asset Connector and handles progress updates the AMR Physical Asset receives whenever a Functionality is finished.

<!--
- simple
- only one Transport Order at a time, no queuing
- send the vehicles Description containing information about kinematics, general vehicle properties and special abilities to the AMR Logical Agent
- report AMR Status Updates consiting of the AMR's state (idle, working, error) and position to the AMR Logical Agent<br />
  will be offered other participants
- manage the execution of one [Transport Order](../glossary.md#t) at a time.
  Transport Orders are described in [Material Flow](./material_flow.md).
- report AMR Transport Order Updates, which are events that occurred related to the execution of Transport Order, to the AMR Logical Agent
- forward Transport Order Steps to execute to the real AMR as described in [Communication with real or simulated robot](#communication-with-real-or-simulated-robot)
-->

<figure markdown>
  ![**Figure 1:** AMR Physical Asset's Components](../img/amr_physical_asset.png)
  <figcaption markdown>**Figure 1:** Components and messages</figcaption>
</figure>

### Functionality

A Functionality is a simple representation of an action the robot can perform.
There are 4 types of Functionalities that are used to execute Order Steps:

- **Move To**: Move to a position.
  This does not differentiate between empty movement and transporting a payload and is used to execute a Transport Order Step or Move Order Step.
- **Load**: Load a payload at the current position and is used to execute a Transport Order Step or Action Order Step.
- **Unload**: Unload a payload at the current position and is used to execute a Transport Order Step or Action Order Step.
<!-- TODO Navigate is unused. Delete? -->
- **Navigate**: Move sequentially to multiple waypoints.
  This does not differentiate between empty movement and transporting a payload.

### Finite State Machine (FSM)

The Finite State Machine represents the state of the current Order.
It will start in state _10 Finished_, which corresponds to the AMR's idle state.
The Finite State Machine assumes to process a Transport Order that is part of a Material Flow created by a Material Flow Logical Agent and was already queued by the AMR Logical Agent.
Therefore the first state it enters is _1 Started_.

<figure markdown>
  ![**Figure 2:** Finite State Machine (FSM)](../img/amr_physical_asset_fsm_current.png)
  <figcaption markdown>**Figure 1:** Components and messages</figcaption>
</figure>

The AMR executes Functionalities in states:

- _2 GoToPickUpLocation:_ MoveTo pickup location
- _4 Load:_ Load payload at pickup location
- _6 GoToDeliveryLocation:_ MoveTo delivery location
- _8 Unload:_ Unload payload at delivery location

Some states will only be used to notify the AMR Logical Agent and then be traversed to the next state that executes a functionality.
Those are:

- _1 Started:_ Order execution has started
- _3 ReachedPickUpLocation:_ MoveTo has finished, pickup location reached
- _5 Loaded:_ Load payload has finished
- _7 ReachedDeliveryLocation:_ MoveTo has finished, delivery location reached
- _9 Unloaded:_ Unload has finished
- _10 Finished:_ Order execution has finished, the AMR is idle and can execute the next Order

All states are transitioned in order from 1 Started to 10 Finished for a Transport Order.
There are extra transitions between:

- _5 Loaded_ and _2 GoToPickUpLocation:_ because a Transport Order can load payload multiple times
- _10 Finished_ and _1 Started:_ because the FSM will remain in _10 Finished_ for the last executed Order until another Order to execute is received

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

- AMR Asset Connector
- The interface has to be implemented
- AMR Physical Asset will send Order Steps to the implementation and expect to receive a completion message
- TODO Asset Connector Picture
<!-- * error messages currently not available/planned -->

<figure markdown>
  ![**Figure 3:** AMR Asset Connector](../img/amr_asset_connector.png)
  <figcaption markdown>**Figure 1:** Components and messages</figcaption>
</figure>

## Mobility

- calculated in AMR Logical Agent
- trapezoid model
- loading and unloading times
- AMR Mobility Helper (simulated mobility, estimations for planning)
- data from the real robot

