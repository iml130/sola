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

#### ``number_nodes``

The number of SOLA nodes available in the simulation.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `3`
    - Data type: ``uint64_t``

#### ``scenario_sequence``

A dictionary containing a list of possible scenario sequence steps.
The steps are executed in the same order as they appear in the sequence.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: `start_sola`, `subscribe_topic`, `delay`, `publish`

#### ``scenario_sequence.start_sola``

Start and initialize SOLA on **all nodes**.
Starting SOLA means, depending on the selected SOLA components, to connect to other nodes.

This is a dictionary containing key, value pairs, that further specify this step.

??? properties

    - Required:
    - Possible values: `delay`

#### ``scenario_sequence.start_sola.delay``

Delay between the nodes joining the SOLA network.

!!! danger "Default delay not added"

    Only the delay specified with `delay` is added between the start of SOLA on different nodes.
    The default delay is not added!
    Further, this delay is not added after the last schedule of start!

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``std::string``
    - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.

#### ``scenario_sequence.subscribe_topic``

Specifies that **all** nodes should subscribe to a topic.

This is a dictionary containing key, value pairs, that further specify this step.

??? properties

    - Required:
    - Possible values: `topic`, `delay`

#### ``scenario_sequence.subscribe_topic.topic``

The name of the topic to subscribe to.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `helloworld`
    - Data type: ``std::string``

#### ``scenario_sequence.subscribe_topic.delay``

Delay between the nodes joining the SOLA network.

!!! danger "Default delay not added"

    Only the delay specified with `delay` is added between the subscription process of different nodes.
    The default delay is not added!
    Further, this delay is not added after the last scheduling of a subscribe event!

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``std::string``
    - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.

#### ``scenario_sequence.delay``

This step allows to add delay between steps.

This is a dictionary containing key, value pairs, that further specify this step.

??? properties

    - Required:
    - Possible values: `delay`

#### ``scenario_sequence.delay.delay``

The delay.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``std::string``
    - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.

#### ``scenario_sequence.publish``

This step allows to let a node publish a message on a topic.

This is a dictionary containing key, value pairs, that further specify this step.

??? properties

    - Required:
    - Possible values: `topic`, `message_size`, `node_id`

#### ``scenario_sequence.publish.topic``

The name of the topic to which the message should be published to.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``std::string``

#### ``scenario_sequence.publish.message_size``

The size of the message to publish in bytes.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``uint64_t``

#### ``scenario_sequence.publish.node_id``

The node that should publish a message.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``uint64_t``
