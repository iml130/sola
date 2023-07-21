# Using SOLA

- Chapter structure suggested
- TODO: All content is to be regarded a placeholder.

## Quickstart

- `sola`
    - `./daisi_exec --environment=sola --scenario=daisi/scenarios/sola/default.yml`

Each example command assumes that the simulation executable is located in the main folder, but can of course be adapted to other names and locations.
The default location is `build/daisi/src/main/Main`.
As shown, the environment for a simulation run can be selected with the `environment` parameter.
Additionally, a scenario has to be defined with the `scenario` parameter.

## Szenarios

No matter what [run environment](../sola/environments.md) is chosen when running DAISI, all environments use a scenario file with some fields that are always required.

### Szenaro file structure

```yaml
# required
title: Title
name: Test with fanout 2
stoptime: 10000000
version: 0.1
physicalLayer: ETH
fanout: 2

# optional
defaultDelay: 5000
logLevel: info
randomSeed: 1

# required
scenarioSequence:
    ...
```

### Create new szenarios

The `title` and `name` fields are purely descriptive at the moment and not further used.
`stoptime` defines at which simulation time in milliseconds the simulation will be aborted if it does not finish earlier.
The `version` field can be used to differentiate different implementation versions.
With `physicalLayer` we define the protocol for connecting the nodes to each other, by default we use Ethernet (`ETH`), but keep in mind that some simulation parts are unaffected from this setting and use Wi-Fi.
Since we build tree structures, we also have to define the `fanout` for them, which must be in the range m >= 2.

The optional fields include `defaultDelay` for the delay between each simulation step in milliseconds, the `logLevel` that can be either `debug`, `info`, `warning`, or `critical`, as well as the `randomSeed` (0 for random) used for reproducible simulations.

Below those mentioned fields the `scenarioSequence` is required, but what it can contain differs based on the run environment. Please refer to the specific components for details about the `scenarioSequence` ([MINHTON](../daisi_lib/minhton-ns3/management-overlay.md), [natter](../daisi_lib/natter-ns3/event-distribution.md), [CPPS](../daisi_lib/applications/cpps/index.md), [Path Planning](../daisi_lib/applications/path-planning/index.md)).

## Run it

- `cpps`
    - `./daisi_exec --environment=sola --scenario=daisi/scenarios/sola/default.yml`

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
## Academic Attribution

If you use SOLA for research (or any other purposes), please include one of the following references in any resulting publication.

```plain
@inproceedings{detznerSOLADecentralizedCommunication2023,
  title = {{{SOLA}}: {{A Decentralized Communication Middleware Developed}} with ns-3},
  shorttitle = {{{SOLA}}},
  booktitle = {Proceedings of the 2023 {{Workshop}} on ns-3},
  author = {Detzner, Peter and Gödeke, Jana and Tönning, Lars and Laskowski, Patrick and Hörstrup, Maximilian and Stolz, Oliver and Brehler, Marius and Kerner, Sören},
  date = {2023-06-28},
  series = {{{WNS3}} '23},
  pages = {78--85},
  publisher = {{Association for Computing Machinery}},
  location = {{New York, NY, USA}},
  url = {https://dl.acm.org/doi/10.1145/3592149.3592151},
  abstract = {The transformation from static production facilities into a flexible and decentralized cyber-physical production system (CPPS) is part of the current ongoing Industry 4.0. A CPPS will enable and support communication between people, machines and virtual objects, e.g., as material flow or products, alike. However, communication in CPPS relies often on centralized approaches using a message broker or is not considered at all. We present in this paper the decentralized communication middleware called SOLA with an emphasis on, but not limited to, CPPS. SOLA uses the inherent given capability to communicate of participating nodes and eliminates the need for a central instance. A structured overlay network is created and managed, which appears to its users as a single coherent system. The main building blocks of SOLA are the management overlay and the event dissemination. Within this building blocks, no single peer has a global view and all operations are based on each peer’s local view. The local view represents some selected links to a subset of all peers in the network. In addition to this, we also present how SOLA was developed with the help of the discrete-event simulator ns-3. Finally, we also show how we have used ns-3 to simulate a self-organizing material flow where participants use SOLA to communicate.},
  isbn = {9798400707476},
  keywords = {balanced tree,cyber-physical production system,decentral organized communication,peer-to-peer network}
}
```
