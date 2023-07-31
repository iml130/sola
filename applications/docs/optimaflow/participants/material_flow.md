# Material Flow

## Material Flow Description

TODO linking to MFDL

The MFDL consists of 
- Move Orders
- Action Orders, and
- Transport Orders

At the moment, only one type of task configuration of the MFDL is possible: 
Action Order, Transport Order with two Transport Order Steps, and a final Action Order. 
This represents the process of one AMR traveling to a pickup location, loading the good, transporting the good to a delivery location, and unloading afterwards. 
Other task configurations are not supported yet, but will be in the future. 

## Material Flow Logical Agent

The Material Flow Logical Agent (MF Agent) is a virtual component within the CPPS that manages, handles, and tracks the execution of granular steps in the material flow. 
It is, additionally, responsible for orchestrating the assignments of tasks to AMRs, and can be in charge of one or multiple material flows, depending on the system's architecture. 
The MF Agent communicates with AMR Logical Agents via SOLA. 
Moreover, it supports the implementation of various algorithms to optimize material flow, more than just task assignment. 


## Lifecycle

- TODO rephrase to Material Flow

Each Transport Order will traverse the following states:


1. kCreated: The task creation is performed by the Material Flow Logical Agent.
2. kQueued: Once an AMR is selected for the execution of a Task, it queues the it.
3. kStarted: Once the Task is sent to the AMR Physical Asset it will start the execution. 
4. kGoToPickupLocation: In the current task configuration, the first step is always going to a pickup location. 
   The Task enters this state once it starts moving.
5. kReachedPickupLocation: The AMR has reached the pickup location.
6. kLoad: The Task enters this state once the AMR starts loading the payload.
7. kLoaded: The AMR has loaded the payload. After loading the AMR will go to a delivery location.
8. kGoToDeliveryLocation: The Task enters this state once the AMR starts going to the delivery station.
9. kReachedDeliveryLocation: The AMR has reached the delivery location.
10. kUnload: The Task enters this state once the AMR starts loading the payload.
11. kUnloaded: The AMR has unloaded the payload. This is the last step in a Task, so the following state will always be finished.
12. kFinished: There are no more steps left in the Task.
13. kError: This state is reached if the Transport Order cannot be finished.
    This is the case if
    - another Transport Order is currently executed
    <!-- - the AMR is not able to execute the Transport Order because it is not designed to handle the payload (VDA 5050) -->
    - the AMR has a technical problem and enters an error state

A Transport Order is created in a Material Flow Logical Agent, will be queued in an AMR Logical Agent and will traverse all other states in an AMR Physical Asset as shown in **Figure 2**.
All state changes will be reported upwards to the Material Flow Logical Agent.

<figure markdown>
  ![**Figure 2:** States a Transport Order traverses, grouped by components that cause the transition.](../img/amr_transport_order_flowchart.drawio.png)
  <figcaption markdown>**Figure 2:** States a Transport Order traverses, grouped by components that cause the transition.</figcaption>
</figure>
<!-- TODO rename file -->
