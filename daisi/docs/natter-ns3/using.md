# Using NATTER

- Chapter structure suggested
- TODO: All content is to be regarded a placeholder.

## Quickstart

- `natter`
    - `./daisi_exec --environment=natter --scenario=daisi/scenarios/natter/default.yml`

Each example command assumes that the simulation executable is located in the main folder, but can of course be adapted to other names and locations.
The default location is `build/daisi/src/main/Main`.
As shown, the environment for a simulation run can be selected with the `environment` parameter.
Additionally, a scenario has to be defined with the `scenario` parameter.

## Szenarios

### Szenaro file structure

```yaml
# required:  
title: Test
name: ED test with 5 publish operations
stoptime: 4294967296
version: 0.1
physicalLayer: ETH
fanout: 2

# optional
defaultDelay: 500
logLevel: info
randomSeed: 1 # 0 random, otherwise...

# required:
numberNodes: 20
mode: minhcast

scenarioSequence: 
    - join:
        mode: minhton
    - publish:
        number: 5
        delay: 0
        mode: random # random, sequential
        message_size: 100 # in bytes
```

### Create new szenarios

The Event Dissemination (ED) currently supports the following mode:

- minhcast (MINHCAST)

This **must** be declared in the scenario file.

In addition, the simulation environment also requires a sequence, which instructs the simulator what happens in which order.

- In the beginning the root node already exists. This does not need to be declared.
- Sequences are executed sequentially.

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
