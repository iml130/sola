<!-- This is a template for the general keys in the scenariofile.
As Markdown does not handle includes, this part must be copied to the top of each scenariofile documentation. -->

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
