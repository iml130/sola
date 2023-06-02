# Path Planning

An example for a scenario file for simulation configuration can be found in the [default.yml for Path Planning](https://github.com/iml130/sola/blob/main/daisi/scenarios/path_planning/default.yml).

## Current limitations

* Number of (paxos) consensus participants must be known at start time and must be static
* Only pickup stations can participate in Consensus/Paxos
* Fixed purely time based collision avoidance at intersections (dimensions and current kinematics are disregarded)
* Using perfect ARP
* If the number of AMRs is not evenly dividable through the number of pickup stations, the AMRs are assigned as equally distributed as possible. No other strategy can be selected.

## Message IDs

* 1 -> DriveMessage
* 2 -> NewAuthorityAGV
* 3 -> ReachedGoal
* 4 -> CentralRequest
* 5 -> CentralResponse

## TO States

* 0 -> SPAWNED
* 1 -> CONSENSUS START
* 2 -> CONSENSUS END
* 3 -> START DRIVE
* 4 -> UNLOADING TO FINISHED
* 5 -> CONSENSUS FAILED (ALREADY OTHER IN PROGRESS FOR SAME STATION)
* 6 -> CONSENSUS FAILED (DUE TO CONSENSUS OR NO FREE ROUTES)
