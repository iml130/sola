# Material Flow

## Structure

- Material Flow
- Task
- Transport Order 
- Action Order, Move Order? (Thoose are not in use))
- (Transport, Action, Move) Order Step
TODO Figure

## Logical Agent

- Task Assignment
   - Algorithms (Link?)
   - Interface
- Task Status tracking

## Lifecycle

- TODO rephrase to Material Flow

Each Transport Order will traverse the following states:

1. kCreated: Transport Orders are created with the Material Flow and Task they are a part of.
   The creation is performed by the Material Flow Logical Agent.
2. kQueued: Once an AMR is selected for the execution of a Task, it queues the Task and its contained Transport Orders.
   The queueing is performed by the AMR Logical Agent.
3. kStarted: Once the Transport Order is sent to the AMR Physical Asset it will start execution the first Transport Order Step.
4. kGoToPickupLocation: The first Transport Order Step is always to go to a pickup location.
   The Transport Order enters this state once it starts moving.
5. kReachedPickupLocation: The AMR has reached the pickup location.
6. kLoad: The Transport Order enters this state once the AMR starts loading the payload.
7. kLoaded: The AMR has loaded the payload.
   After loading the AMR will either go to a delivery location or go to another pickup location.
8. kGoToDeliveryLocation: The Transport Order enters this state once the AMR starts going to the delivery station.
9. kReachedDeliveryLocation: The AMR has reached the delivery location.
10. kUnload: The Transport Order enters this state once the AMR starts loading the payload.
11. kUnloaded: The AMR has unloaded the payload. This is the last step in a Transport Order so the following state will always be finished.
12. kFinished: There are no more steps left in the Transport Order.
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
