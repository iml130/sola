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

#### ``stop_time``

Defines at which simulation time the simulation will be aborted if it does not finish earlier.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``std::string``
    - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.

#### ``version``

Can be used to differentiate different implementation versions.
Currently not used.

??? properties

    - Required:
    - Reasonable default: `0.1`
    - Data type: ``std::string``

#### ``default_delay``

Delay between each simulation step (operations defined in the `scenario_sequence`, respectively between each step for *x*-many operations).

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``std::string``
    - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.

#### ``random_seed``

Seed used for reproducable simulations.
If ``=0`` random results are produced.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `1`
    - Data type: ``uint64_t``

#### ``output_path``

Directory path for the resulting logging output.

??? properties

    - Required: Only if environment variable `DAISI_OUTPUT_PATH` is not set. This yml paramater takes precedence if both are set.
    - Reasonable default: `/work/ns3/results/`
    - Data type: ``std::string``

<!-- END COPY GENERAL -->

#### ``fanout``

The fanout of the underlying MINHTON tree used for broadcasting.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `2`
    - Data type: ``uint64_t``
    - Possible values: any integer ``>= 2``

#### ``number_nodes``

The number of nodes in the MINHTON tree used for broadcasting.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `20`
    - Data type: ``uint32_t``

#### ``mode``

The natter algorithm used for broadcasting.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `minhcast`
    - Data type: ``std::string``
    - Possible values: ``minhcast``

#### ``scenario_sequence``

A dictionary containing a list of possible scenario sequence steps.
The steps are executed in the same order as they appear in the sequence.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``join``, ``publish``

#### ``scenario_sequence.join``

A scenario step to let nodes join the MINHTON tree.
When executing this step, all nodes (as specified by ``numberNodes``) will join the network.

This is a dictionary containing key, value pairs, that further specify this step.

??? properties

    - Required:
    - Possible values: ``mode``, ``delay``

#### ``scenario_sequence.join.mode``

Specify which type of join should be used to join the tree.

With the MINHCAST join, all connections are statically calculated and inserted into the nodes.
The [actual MINHTON join algorithm](../../minhton/algorithms/join.md) is not executed.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``minhcast``

#### ``scenario_sequence.join.delay``

Specifies **additional** delay (added to ``default_delay``) that is added **before** executing the MINHTON static join.

??? properties

    - Required:
    - Data type: ``std::string``
    - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.

#### ``scenario_sequence.publish``

A scenario step to let nodes publish a message.

This is a dictionary containing key, value pairs, that further specify this step.

??? properties

    - Required:
    - Possible values: ``number``, ``delay``, ``mode``, ``message_size``

#### ``scenario_sequence.publish.number``

Specifies the number of nodes that should publish a message in this step.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `1`
    - Data type: ``uint64_t``

#### ``scenario_sequence.publish.delay``

Specifies **additional** delay (added to ``default_delay``) that is added **before** before publishing a message.

When more than one node should publish a message in the same step, this delay is added before each publish.

??? properties

    - Required:
    - Data type: ``std::string``
    - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.

#### ``scenario_sequence.publish.mode``

Specifies which node(s) should publish a message.

In mode ``random``, the nodes are selected completely randomly.

With mode ``sequential``, the nodes are selected by their ID one after another, starting with ID 0.
For MINHTON, this results in publishing level-wise from left to right, starting with node 0:0, then 1:0, 1:1, 2:0, 2:1 and so forth.
For each ``publish`` step, this procedure starts again from ID 0.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `random`
    - Data type: ``string``
    - Possible values: ``random``, ``sequential``

#### ``scenario_sequence.publish.message_size``

The size of the message that should be published in bytes.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `100`
    - Data type: ``uint64_t``
