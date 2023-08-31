# ESearch

The Entity Search (ESearch) is a builtin functionality of MINHTON to search for specified data in the network.
For simulation purposes, you specify the ESearch executions in the scenario file used by MINHTON.

## Initial Scenario File

Below the [usual scenario file content](management-overlay.md), you can specify the ESearch find query inside a scenario sequence.
You initiate a find query with the requesting node (level:number). The scope can be `all` or `some` and the query must be a valid expression.
In the following example we define a find query from the requesting node 100:100 with an unrestricted scope and query the attribute `a01` and `a02` with a value over 5.

```yaml
...
scenarioSequence:
...
    - find-query:
        level: 100
        number: 100
        scope: all
        query: ((HAS a01) AND (a02 > 5))
        validity-threshold: 10000 
```

Periodically repeated requests are being counted down. After the countdown value has been reached, the simulation will wait until the last request has been processed and the simulation stop time will be set accordingly.

```yaml
- request-countdown: 1000
```

## Peer Discovery Commands

Peer Discovery can be turned off by using

```yaml
peerDiscoveryEnvironment:
    attributes:
    requests:
```

## Attributes / Values

The name of an attribute (here a01, a02...) can be chosen arbitrarily, but must be unique.

Each attribute needs a presence, content and update behavior.

Presence behavior describes which nodes posses the attribute and which not.

Content behavior describes the kinds of values an attribute can have.

Update behavior describes how often the value of an attribute will be updated.

```yaml
attributes:
    a01:
        presence_behavior:
            ...
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

### Presence Behavior

Presence Behavior is simply described by the percentage of nodes who posses this attribute. The percentage has to be given as a float (100% -> 1.0).

```yaml
presence_behavior:
    percentage: 0.8
```

### Content Behavior

Uniform and Gaussian are only numerical values.
Choice and constant can be either integers, floats, booleans or strings.

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

### Update Behavior

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
