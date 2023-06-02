# Scenario Files

No matter what [run environment](../sola/environments.md) is chosen when running DAISI, all environments use a scenario file with some fields that are always required.

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

The `title` and `name` fields are purely descriptive at the moment and not further used.
`stoptime` defines at which simulation time in milliseconds the simulation will be aborted if it does not finish earlier.
The `version` field can be used to differentiate different implementation versions.
With `physicalLayer` we define the protocol for connecting the nodes to each other, by default we use Ethernet (`ETH`), but keep in mind that some simulation parts are unaffected from this setting and use Wi-Fi.
Since we build tree structures, we also have to define the `fanout` for them, which must be in the range m >= 2.

The optional fields include `defaultDelay` for the delay between each simulation step in milliseconds, the `logLevel` that can be either `debug`, `info`, `warning`, or `critical`, as well as the `randomSeed` (0 for random) used for reproducible simulations.

Below those mentioned fields the `scenarioSequence` is required, but what it can contain differs based on the run environment. Please refer to the specific components for details about the `scenarioSequence` ([MINHTON](../daisi_lib/minhton-ns3/management-overlay.md), [natter](../daisi_lib/natter-ns3/event-distribution.md), [CPPS](../daisi_lib/applications/cpps/index.md), [Path Planning](../daisi_lib/applications/path-planning/index.md)).
