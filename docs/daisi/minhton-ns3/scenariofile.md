# Scenariofile

!!! danger "This is work in progress!"

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

#### ``fanout``

The [fanout](../../minhton/glossary.md#f) of the MINHTON tree.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Reasonable default: `2`
    - Data type: ``uint64_t``
    - Possible values: any integer ``>= 2``

#### `timeouts`

Map to configure timeouts of different algorithms.
Values are in miliseconds.
For information about the different keys, see the [Doxygen page](https://iml130.github.io/sola/doxygen/structminhton_1_1TimeoutLengthsContainer.html).

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: see [Doxygen](https://iml130.github.io/sola/doxygen/structminhton_1_1TimeoutLengthsContainer.html)

#### `algorithms`

Map to configure which algorithms should be used.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``join``, ``leave``, ``search_exact``, ``response``, ``bootstrap``

#### `algorithms.join`

Algorithm to use for joining the network.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: `minthon`
    - Data type: ``std::string``

#### `algorithms.leave`

Algorithm to use for leaving the network.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: `minthon`
    - Data type: ``std::string``

#### `algorithms.search_exact`

Algorithm to use to search for a specific node.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: `minthon`
    - Data type: ``std::string``

#### `algorithms.response`

Algorithm to use for responding to general messages.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: `general`
    - Data type: ``std::string``

#### `algorithms.bootstrap`

Algorithm to use for bootstrapping (finding entry point to network).

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: `general`
    - Data type: ``std::string``

#### `peer_discovery_environment`

Configuration for Peer Discovery.
Please refer to the [examples below](#peer-discovery) for detailed information on how to configure Peer Discovery.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``attributes``, ``requests``

#### `peer_discovery_environment.attributes`

<!-- TODO -->

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``off``, ``on``, or a list of attributes with more keys like in the [example scenariofile for entity search](https://github.com/iml130/sola/blob/main/daisi/scenarios/entity_search/example.yml).

#### `peer_discovery_environment.requests`

<!-- TODO -->

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``off``, ``on``, or a list of attributes with more keys like in the [example scenariofile for entity search](https://github.com/iml130/sola/blob/main/daisi/scenarios/entity_search/example.yml).

#### ``scenario_sequence``

A dictionary containing a list of possible scenario sequence steps.
The steps are executed in the same order as they appear in the sequence.
In the beginning the root node already exists.
This does not need to be declared.

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Possible values: ``join-one``, ``join-many``, ``leave-one``, ``leave-many``, ``search-many``, ``search-all``, ``fail-one``, ``fail-many``, ``mixed-execution``, ``validate-leave``, ``find-query``,  ``time``, ``static-build``, ``request-countdown``

#### `scenario_sequence.join-one`

Creates a new node that joins the network on a specified existing node defined by its ``index`` or by ``level`` and ``number``.
When ``level`` + ``number`` and ``index`` are both defined, ``level`` + ``number`` take precedence.

??? properties

    - ``delay``
        - Required:
        - Data type: ``std::string``
        - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.
    - ``level``
        - Required: :fontawesome-solid-star-of-life: (Together with ``number``. Alternatively define the ``index``.)
        - Data type: ``uint32_t``
    - ``number``
        - Required: :fontawesome-solid-star-of-life: (Together with ``level``. Alternatively define the ``index``.)
        - Data type: ``uint32_t``
    - ``index``
        - Required: :fontawesome-solid-star-of-life: (Alternatively define ``level`` and ``number``.)
        - Data type: ``uint32_t``

#### `scenario_sequence.join-many`

Creates multiple new nodes (with the amount given by `number`) that join the network.
Depending on the ``mode``, the new nodes either join on a node randomly selected each time, always on the root node, or they use the discover functionality to find an existing node.

??? properties

    - ``delay``
        - Required:
        - Data type: ``std::string``
        - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.
    - ``number``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``uint32_t``
    - ``mode``
        - Required: :fontawesome-solid-star-of-life:
        - Possible values: ``random``, ``root``, ``discover``
        - Data type: ``string``

#### `scenario_sequence.leave-one`

Selects an existing node by its ``index`` or by ``level`` and ``number`` and initiates its leaving procedure.
When ``level`` + ``number`` and ``index`` are defined, ``level`` + ``number`` take precedence.

??? properties

    - ``delay``
        - Required:
        - Data type: ``std::string``
        - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.
    - ``level``
        - Required: :fontawesome-solid-star-of-life: (Together with ``number``. Alternatively define the ``index``.)
        - Data type: ``uint32_t``
    - ``number``
        - Required: :fontawesome-solid-star-of-life: (Together with ``level``. Alternatively define the ``index``.)
        - Data type: ``uint32_t``
    - ``index``
        - Required: :fontawesome-solid-star-of-life: (Alternatively define ``level`` and ``number``.)
        - Data type: ``uint32_t``

#### `scenario_sequence.leave-many`

Initiates the leave procedure on multiple existing nodes (with the amount given by `number`).
Depending on the ``mode``, the nodes are either randomly selected each time, or the leave is always initiated on the root node at the given time.

??? properties

    - ``delay``
        - Required:
        - Data type: ``std::string``
        - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.
    - ``number``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``uint32_t``
    - ``mode``
        - Required: :fontawesome-solid-star-of-life:
        - Possible values: ``random``, ``root``
        - Data type: ``string``

#### `scenario_sequence.search-many`

Searches for multiple nodes (with the amount given by `number`).

??? properties

    - ``delay``
        - Required:
        - Data type: ``std::string``
        - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.
    - ``number``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``uint32_t``

#### `scenario_sequence.search-all`

Searches for all nodes from all nodes, meaning that for `N` existing nodes this leads to `N^2 - N` initiated searches.
Warning: Can quickly congest the network!

??? properties

    - ``delay``
        - Required:
        - Data type: ``std::string``
        - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.

#### `scenario_sequence.fail-one`

Initiates a failure an existing node specified by its ``index`` or by ``level`` and ``number``.
When ``level`` + ``number`` and ``index`` are defined, ``level`` + ``number`` take precedence.

??? properties

    - ``delay``
        - Required:
        - Data type: ``std::string``
        - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.
    - ``level``
        - Required: :fontawesome-solid-star-of-life: (Together with ``number``. Alternatively define the ``index``.)
        - Data type: ``uint32_t``
    - ``number``
        - Required: :fontawesome-solid-star-of-life: (Together with ``level``. Alternatively define the ``index``.)
        - Data type: ``uint32_t``
    - ``index``
        - Required: :fontawesome-solid-star-of-life: (Alternatively define ``level`` and ``number``.)
        - Data type: ``uint32_t``

#### `scenario_sequence.fail-many`

Initiates a failure on multiple nodes (with the amount given by `number`).

??? properties

    - ``delay``
        - Required:
        - Data type: ``std::string``
        - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.
    - ``number``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``uint32_t``

#### `scenario_sequence.mixed-execution`

Schedules a mixed execution with joins (with the amount given by ``join-number``), leaves (with the amount given by ``leave-number``), and searches (with the amount given by ``search-number``) that are randomly shuffled.

??? properties

    - ``delay``
        - Required:
        - Data type: ``std::string``
        - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.
    - ``join-number``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``uint32_t``
    - ``leave-number``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``uint32_t``
    - ``search-number``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``uint32_t``

#### `scenario_sequence.validate-leave`

Iterates over all nodes and lets them leave, followed by them joining the network again in the mode ``random``.

??? properties

    - ``delay``
        - Required:
        - Data type: ``std::string``
        - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.

#### `scenario_sequence.find-query`

Selects a requesting node by its ``level`` and ``number`` that sends a find query.
The find query is specified by ``scope``, ``query``, and ``validity-threshold``.

??? properties

    - ``level``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``uint32_t``
    - ``number``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``uint32_t``
    - ``scope``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``string``
    - ``query``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``string``
    - ``validity-threshold``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``uint32_t``

#### `scenario_sequence.time`

Waits the specificed amount of ``time``.

??? properties

    - ``time``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``std::string``
        - Possible values: Any string ending with a unit prefix like "s" or "ms". If no unit is given, it will be treated as seconds.

#### `scenario_sequence.static-build`

Creates a network with multiple nodes (with the amount given by ``number``) statically without using the join algorithm.

??? properties

    - ``number``
        - Required: :fontawesome-solid-star-of-life:
        - Data type: ``uint32_t``

#### `scenario_sequence.request-countdown`

<!-- TODO -->

??? properties

    - Required: :fontawesome-solid-star-of-life:
    - Data type: ``uint64_t``

## Examples

### Adding Nodes

The following `scenario_sequence` lets 100 nodes join the network.

```yaml
scenario_sequence:
    - type: join-many:
        number: 100
        mode: random
        delay: 1s
```

### Removing Nodes

The following `scenario_sequence` lets 100 nodes join the network, and than 100 nodes are leaving the network.

```yaml
scenario_sequence:
    - type: join-many:
        number: 100
        mode: random
        delay: 1s
    - type: leave-many:
        number: 100
        mode: random
        delay: 1s
```

### Searching Nodes

The following `scenario_sequence` lets 100 nodes join the network, and than 100 search-exact queries are executed.
After 100 search-exact queries are executed, "search-all" is executed (Warning: search-all can congest the network quickly).

```yaml
scenario_sequence:
    - type: join-many:
        number: 100
        mode: random
        delay: 1s
    - type: search-many:
        number: 100
        delay: 1s
    - type: search-all:
        delay: 1s
```

### Static P2P Build

Building a network statically of 100 nodes + 1 root node without needing join.
It is only possible to execute this command once right in the beginning.
The event timestamp cannot be 0 - therefore you need to increase the time by a little bit.

```yaml
scenario_sequence:
    - type: time
        time: 100
    - type: static-build:
        number: 100
        mode: random
        delay: 1s
```

### Peer Discovery

Inside the scenario sequence, [ESearch](../../minhton/using.md#peer-discovery-entity-search-esearch) find queries can be specified, after you have created a network as [shown above](#adding-nodes).
You initiate a find query with the requesting node (level:number).
The scope can be `all` or `some` and the query must be a valid expression.
In the following example we define a find query from the requesting node 100:100 with an unrestricted scope and query the attribute `a01` and `a02` with a value over 5.

```yaml
...
scenario_sequence:
...
    - type: find-query
        level: 100
        number: 100
        scope: all
        query: ((HAS a01) AND (a02 > 5))
        validity-threshold: 10000
```

Periodically repeated requests are being counted down.
After the countdown value has been reached, the simulation will wait until the last request has been processed and the simulation stop time will be set accordingly.

```yaml
scenario_sequence:
...
    - type: request-countdown
        number: 100
```

Peer Discovery can be turned off by using

```yaml
peer_discovery_environment:
    attributes: off
    requests: off
```

To enable Peer Discovery, you need to specify the attributes.
The name of an attribute (here a01, a02...) can be chosen arbitrarily, but must be unique.

Each attribute needs a presence, content and update behavior like this:

```yaml
attributes:
    a01:
        presence_behavior:
            percentage: 0.8
        content_behavior:
            ...
        update_behavior:
            ...
    a02:
        presence_behavior:
            ...
        content_behavior:
            ...
        update_behavior:
            ...
```

**Presence Behavior:**

Presence behavior describes the percentage of nodes who posses this attribute.
The percentage has to be given as a float (100% -> 1.0).

**Content Behavior:**

Content behavior describes the kinds of values an attribute can have.
With the types `uniform` and `gaussian` you can only use numerical values.
With the types `choice` and `constant` the values can be either integers, floats, booleans or strings.

Uniform:

```yaml
content_behavior:
    type: uniform
    min: 0
    max: 10
```

Gaussian:

```yaml
content_behavior:
    type: gaussian
    mean: 10
    variance: 2
```

Constant:

```yaml
content_behavior:
    type: constant
    value: 42
```

Choice:

```yaml
content_behavior:
    type: choice
    values:
        - value1:
            content: pallet
            prob: 0.4
        - value2:
            content: box
            prob: 0.6
```

**Update Behavior:**

Update behavior describes how often the value of an attribute will be updated.
The update behavior is either dynamic or static.
If its static, the value will never be updated.
If its dynamic, values will be updated periodically.
The time period between updates is given in milliseconds, either by a constant or gaussian distribution.

Constant:

```yaml
update_behavior:
    type: constant
    value: 20000 # ms
```

Gaussian:

```yaml
update_behavior:
    type: gaussian
    mean: 20000 # ms
    variance: 5000 # ms
```

Uniform:

```yaml
update_behavior:
    type: uniform
    min: 20000 # ms
    max: 40000 # ms
```

Static:

```yaml
update_behavior:
    type: static
```
