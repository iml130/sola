# Using OptiMaFlow

- Chapter structure suggested
- TODO: All content is to be regarded a placeholder.
- "implementation highlights" -> [Mobility Model](./mobility_model.md)
    - Mobility Model/Asset Connector
    - Networking
    - Threading

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

## Szenarios

### Szenaro file structure

### Create new szenarios

## Logging

See [OptiMaFlow Logging](../../../applications/docs/optimaflow/logging.md)

## Interfaces

How the interfaces are implemented is described in [Implementations](mobility_model.md).
