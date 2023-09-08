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
