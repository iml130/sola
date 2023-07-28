# Central
- Idea: Logic of task assignment completely centralized
- heterogenous Round Robin
- Simple Approach with no respect to AMR metrics
- Baseline for further central algorithms

## Concept

UML-class chart here

### CentralizedParticipant
- Simple Participant
- Implements methods to respond to 
    - Assignment Notification
    - Status Update Request

### CentralizedInitiator
- Abstract class 
- represents central task assignment algorithm

### RoundRobinInitiator
- Central task allocation algorithm
- implements (modified) round robin strategy
    - example (img)
- Priority queue per task ability
- knows all participants
- no respect to participant metrics
- ParticipantInfoRoundRobin: struct to store all algorithm-relevant data per participant


## Interaction
- Task Assignnment starts when RoundRobinInitiator receives MFDLScheduler (addMaterialFlow)
    - invoked by CppsManager
    - for each task, assignTask(task) is called
        - calls `chooseParticipantForTask` where the specific task assignment strategy is implemented (Round Robin)
        - sends message `AssignmentNotification` to the chosen participant
- Waits a specific delay for all answers
- CentralizedParticipant sends `AssignmentResponse` after receiving the task assignment
    - always true for the `SimpleOrderManagement`
- All unaccepted task assignments are rescheduled
    - should only happen if there was package loss / node failure

