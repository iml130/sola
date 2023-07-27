# Introduction

## What is ns-3

> ns-3 is a discrete-event network simulator for Internet systems, targeted primarily for research and educational use. The goal of the ns-3 project is to develop a preferred, open simulation environment for networking research: it should be aligned with the simulation needs of modern networking research and should encourage community contribution, peer review, and validation of the software.

Our application runs on top of the [ns-3](https://www.nsnam.org/) network simulator and simulates the chosen scenario in an environment.

## Benefits of using ns-3

- discrete simulation
    - event driven
    - performance
    - logging in distributed systems
- continuous simulation
    - logfile generation i.e. for visualization
    - integration with Ros2?
    - integration with real robots?
- network abstraction and simulation
    - stationary devices
    - moving devices
    - connection strength simulation

## What is ns-3 used for

- modular testing and verification of
    - SOLA
    - MINHTON
    - NATTER
    - Applications
- holistic simulation

## What is ns-3 not designed for

- physical simulation
    - collision detection
    - simulation of objects, their parts and their interaction
- visualization
    - visulization during simulation
    - although continuous logfiles can be visualized in extensions

## Getting started

- Install [required dependencies from SOLA indroduction](../../docs/index.md#getting-started) first?
- Try [Using OptiMaFlow](./optimaflow-ns3/using.md)
- Or use other environments
### Using Environments

- Several environments can be built and used. 
    - [SOLA](./sola-ns3/using.md)
    - [MINHTON](./minhton-ns3/using.md)
    - [NATTER](./natter-ns3/using.md)
    - [OptiMaFlow](./optimaflow-ns3/using.md)

