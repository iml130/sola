# Scenario File

The Event Dissemination (ED) currently supports the following mode:

- minhcast (MINHCAST)

This **must** be declared in the scenario file.

In addition, the simulation environment also requires a sequence, which instructs the simulator what happens in which order.

- In the beginning the root node already exists. This does not need to be declared.
- Sequences are executed sequentially.

## Initial Scenario File

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
