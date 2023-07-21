# Using MINHTON

- Chapter structure suggested
- TODO: All content is to be regarded a placeholder.

## Quickstart

- `minhton`
    - `./daisi_exec --environment=minhton --scenario=daisi/scenarios/minhton/default.yml`

Each example command assumes that the simulation executable is located in the main folder, but can of course be adapted to other names and locations.
The default location is `build/daisi/src/main/Main`.
As shown, the environment for a simulation run can be selected with the `environment` parameter.
Additionally, a scenario has to be defined with the `scenario` parameter.

## Szenarios

The Management Overlay (MO) allows to create a Peer-to-Peer (P2P) Network.
This creation and maintenance can be simulated by ns-3.
Scenario files are used for this purposes and describe what exactly will be simulated in the [YAML](https://yaml.org/) format.

Currently, the MO supports the following creational pattern:

<!--- `baton_star` [[1]](#references) (BATON\*; height-balanced tree)
- `nbaton_star` [[2]]((#references)) (nBATON\*; null-balanced tree; uses DSNs to support Join and Leave operations)-->
- `minhton` [[1]](#references) (MINHTON; complete-balanced tree with support for concurrent operations)

The MINHTON algorithm is based on nBATON\* [[2]](#references), which in turn is based on BATON\* [[3]](#references).

This **must** be declared in the scenario file under `algorithms:`.

In addition, the simulation environment also requires a sequence, which instructs the simulator what happens in which order.

- In the beginning the root node already exists. This does not need to be declared.
- Sequences are executed sequentially.

### Szenaro file structure

```yaml
# required
title: MINHTON_default
name: MO test with fanout 2
stoptime: 10000000
version: 0.1
physicalLayer: ETH
fanout: 2

defaultDelay: 5000
logLevel: info
randomSeed: 1 # 0 = random

timeouts:
    bootstrap_response: 500
    join_response: 500
    join_accept_ack_response: 500
    replacement_offer_response: 500
    replacement_ack_response: 500
    dsn_aggregation: 4000
    inquiry_aggregation: 1000

algorithms:
    join: minhton
    leave: minhton
    search_exact: minhton
    response: general
    bootstrap: general

peerDiscoveryEnvironment:
    attributes: off
    requests: off

scenarioSequence:
    ...
```

### Create new szenarios

#### Adding Nodes

The following `scenarioSequence` let 100 nodes join the network. The following `scenarioSequence` let 100 nodes join the network.

```yaml
scenarioSequence:
    - join-many:
        number: 100
        mode: random
        delay: 1000
```

#### Removing Nodes

The following `scenarioSequence` let 100 nodes join the network, and than 100 nodes are leaving the network.

```yaml
scenarioSequence:
    - join-many:
        number: 100
        mode: random
        delay: 1000
    - leave-many:
        number: 100
        mode: random
        delay: 1000
```

#### Searching Nodes

The following `scenarioSequence` let 100 nodes join the network, and than 100 search-exact queries are executed. After 100 search-exact queries are executed, "search-all" is executed.

```yaml
scenarioSequence:
    - join-many:
        number: 100
        mode: random
        delay: 1000
    - search-many:
        number: 100
        delay: 1000
    - search-all:
        delay: 1000
```

#### Static P2P Build

Building a network statically of 100 + 1 nodes without needing join.
It is only possible to execute this command once right in the beginning.
The event timestamp cannot be 0 - therefore you need to increase the time by a little bit.

```yaml
- time: 100
- static-build:
    number: 100
    mode: random
    delay: 1000
```

## Run it

- `cpps`
    - `./daisi_exec --environment=cpps --scenario=daisi/scenarios/cpps/default.yml`

- command
    - `./daisi_exec`
    - `--environment`
    - `--scenario`
        - scenario from file
        - scenario from command line

Each example command assumes that the simulation executable is located in the main folder, but can of course be adapted to other names and locations.
The default location is `build/daisi/src/main/Main`.
As shown, the environment for a simulation run can be selected with the `environment` parameter.
Additionally, a scenario has to be defined with the `scenario` parameter.

## References

[1] P. Laskowski, P. Detzner, und S. Bondorf, „Tree-structured Overlays with Minimal Height: Construction, Maintenance and Operation“, in DisCoTec 2023, Lisbon, Portugal.

[2] P. Detzner, J. Gödeke, and S. Bondorf, “[Low-Cost search in Tree-Structured P2P overlays: The Null-Balance benefit](https://doi.org/10.1109/LCN52139.2021.9525004),” Edmonton, Canada, Oct. 2021.

[3] H. V. Jagadish, B. C. Ooi, K.-L. Tan, Q. H. Vu, und R. Zhang, „Speeding up search in peer-to-peer networks with a multi-way tree structure“, in Proceedings of the 2006 ACM SIGMOD international conference on Management of data  - SIGMOD ’06, Chicago, IL, USA, 2006, S. 1. doi: 10.1145/1142473.1142475.
