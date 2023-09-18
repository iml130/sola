# Scenariofile

<!-- BEGIN COPY GENERAL -->
The scenariofile is a YAML-based file to specifiy the simulation setup.
It can contain the following keys:

#### ``title``

Can be any string, purely descriptive at the moment.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `ScenarioTitle`
    - Data type: ``std::string``

#### ``name``

Can be any string, purely descriptive at the moment.

??? properties

    - Required:
    - Reasonable default: `ScenarioName`
    - Data type: ``std::string``

#### ``stoptime``
<!-- TODO Adjust to stop_time after modification of manager scenariofile parser -->

Defines at which simulation time in milliseconds the simulation will be aborted if it does not finish earlier.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `10000000`
    - Data type: ``uint64_t``

#### ``version``
<!-- TODO deprecated? -->

Can be used to differentiate different implementation versions.
Currently not used.

??? properties

    - Required:
    - Reasonable default: `0.1`
    - Data type: ``std::string``

#### ``defaultDelay``
<!-- TODO Adjust to default_delay after modification of manager scenariofile parser -->

Delay between each simulation step (operations defined in the `scenarioSequence`, respectively between each step for *x*-many operations) in milliseconds.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `5000`
    - Data type: ``uint64_t``

#### ``randomSeed``
<!-- TODO Adjust to random_seed after modification of manager scenariofile parser -->

Seed used for reproducable simulations.
If ``=0`` random results are produced.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `1`
    - Data type: ``uint64_t``

#### ``logLevel``
<!-- TODO Deprecated; Adjust after modification of manager scenariofile parser -->

Level used for logging.

??? properties

    - Required:
    - Reasonable default: `info`
    - Data type: ``std::string``
    - Possible values: ``debug``, ``info``, ``warning``, ``critical``

#### ``outputPath``
<!-- TODO Adjust after modification of manager scenariofile parser -->

Directory path for the resulting logging output.

??? properties

    - Required: Only if environment variable `DAISI_OUTPUT_PATH` is not set. This yml paramater takes precedence if both are set.
    - Reasonable default: `/work/ns3/results/`
    - Data type: ``std::string``

<!-- END COPY GENERAL -->

#### ``stop_time``

Defines at which simulation time in seconds the simulation will be aborted if it does not finish earlier.
This is the successor of ``stoptime`` which is not used by OptiMaFlow.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `5000`
    - Data type: ``uint64_t``

#### ``random_seed``

Seed used for reproducable simulations.
If ``=0`` random results are produced.
This is the successor of ``randomSeed`` which is not used by OptiMaFlow.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `1`
    - Data type: ``uint64_t``

#### ``default_delay``

Delay between each simulation step (operations defined in the `scenarioSequence`, respectively between each step for *x*-many operations) in seconds.
This is the successor of ``defaultDelay`` which is not used by OptiMaFlow.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `5`
    - Data type: ``uint64_t``

#### ``topology``

Start of a sequence of mappings to define the topology of the simulation area (for example a factory hall).
Details about all possible properties can be found [on the Doxygen page](https://iml130.github.io/sola/doxygen/structdaisi_1_1cpps_1_1TopologyScenario.html).

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: see [Doxygen](https://iml130.github.io/sola/doxygen/structdaisi_1_1cpps_1_1TopologyScenario.html)

```
topology:
    width:  50
    height: 20
    depth:   0
```

#### ``initial_number_of_amrs``

Number of AMRs spawned in the simulation.
One AMR consists of two applications (logical AMR agent and physical AMR) running on a single ns-3 node.
Currently the number of AMRs cannot be changed at runtime, hence this is the total number of AMRs during the entire simulation.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `12`
    - Data type: ``uint64_t``

#### ``number_of_material_flows``

Currently unused.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `5`
    - Data type: ``uint64_t``

#### ``number_of_material_flow_agents``

Number of Material flow agents that should be spawned.
For each agent a separate ns-3 node is created.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `5`
    - Data type: ``uint64_t``

#### ``do_material_flow_agents_leave_after_finish``

Specifies whether a Material Flow agent should shutdown (disconnect from all network functionalities) after the material flow was finished.
If set to ``false``, the Material Flow agent stays connected and can be reused for another material flow.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `false`
    - Data type: ``bool``

#### ``algorithm``

Start of a sequence of mappings to specify the algorithms used by the logical agents.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``assignment_strategy``

#### ``algorithm.assignment_strategy``

The algorithm used to assign tasks/orders.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``iterated_auction``, ``round_robin``

#### ``autonomous_mobile_robots``

Map containing the description of different types of AMRs.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``description``

#### ``autonomous_mobile_robots.description``

Map containing the description of a single type of AMR.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``properties``, ``kinematics``, ``load_handling``

#### ``autonomous_mobile_robots.description.properties``

Basic properties about the AMR.
Details about all possible properties can be found [on the Doxygen page](https://iml130.github.io/sola/doxygen/structdaisi_1_1cpps_1_1AmrPropertiesScenario.html).

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: see [Doxygen](https://iml130.github.io/sola/doxygen/structdaisi_1_1cpps_1_1AmrPropertiesScenario.html)


#### ``autonomous_mobile_robots.description.kinematic``

Information about the kinematics of the AMR.
Details about all possible keys can be found [on the Doxygen page](https://iml130.github.io/sola/doxygen/structdaisi_1_1cpps_1_1AmrKinematicsScenario.html).

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: see [Doxygen](https://iml130.github.io/sola/doxygen/structdaisi_1_1cpps_1_1AmrKinematicsScenario.html)

#### ``autonomous_mobile_robots.description.load_handling``

Information about the load handling units of the AMR.
Details about all possible keys can be found [on the Doxygen page](https://iml130.github.io/sola/doxygen/structdaisi_1_1cpps_1_1AmrLoadHandlingScenario.html).

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: see [Doxygen](https://iml130.github.io/sola/doxygen/structdaisi_1_1cpps_1_1AmrLoadHandlingScenario.html)

#### ``material_flows``

Start of a sequence of mapping to specify the material flows.

Currently only placeholder.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``mf``

#### ``material_flows.mf``

Map of information about a specific material flow.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``mfdl_program``, ``friendly_name``

#### ``material_flows.mf.mfdl_program``

Currently unused.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``std::string``


#### ``material_flows.mf.friendly_name``

Human readable name of this material flow.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``std::string``

#### ``scenario_sequence``

A dictionary containing a list of possible scenario sequence steps.
The steps are executed in the same order as they appear in the sequence.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``spawnInfo``

#### ``scenario_sequence.spawnInfo``

Scenario step to initialize and start entities.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``entity``, ``friendly_name``, ``start_time``, ``spawn_distribution``

#### ``scenario_sequence.spawnInfo.entity``

The general type of entity to initialize.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible value: ``amr``, ``mf``
    - Data type: ``std::string``

#### ``scenario_sequence.spawnInfo.friendly_name``

The specific typ of the entity to initialize (for example the friendly name of a AMR type specified with ``autonomous_mobile_robots``)

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``std::string``

#### ``scenario_sequence.spawnInfo.start_time``

<!-- TODO -->

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``std::string``
    - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.

#### ``scenario_sequence.spawnInfo.spawn_distribution``

Map containing information about the spawn distribution.
<!-- TODO What is this distribution doing? -->

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``type``, ``number``, ``mean``, ``sigma``,  ``percentage``

#### ``scenario_sequence.spawnInfo.spawn_distribution.type``

<!-- TODO -->

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``absolute``, ``gaussian``, ``relative``
    - Data type: ``std::string``

#### ``scenario_sequence.spawnInfo.spawn_distribution.number``

For absolute distribution.
<!-- TODO -->

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``uint16_t``

#### ``scenario_sequence.spawnInfo.spawn_distribution.mean``

Mean for gaussian distribution.
<!-- TODO -->

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``double``

#### ``scenario_sequence.spawnInfo.spawn_distribution.sigma``

Standard deviation for gaussian distribution.
<!-- TODO -->

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``double``

#### ``scenario_sequence.spawnInfo.spawn_distribution.percentage``

Percentage for relative distribution.
<!-- TODO -->

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``double``

