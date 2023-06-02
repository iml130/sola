# Run Environments

The currently available environments (with an example command to run each) are:

- `minhton`
    - `./daisi_exec --environment=minhton --scenario=daisi/scenarios/nbaton/default.yml`
- `natter`
    - `./daisi_exec --environment=natter --scenario=daisi/scenarios/natter/default.yml`
- `cpps`
    - `./daisi_exec --environment=cpps --scenario=daisi/scenarios/cpps/default.yml`
- `path_planning`
    - `./daisi_exec --environment=path_planning --scenario=daisi/scenarios/path_planning/default.yml`
- `sola` (Still in development)
    - `./daisi_exec --environment=sola --scenario=daisi/scenarios/sola/default.yml`

Each example command assumes that the simulation executable is located in the main folder, but can of course be adapted to other names and locations.
The default location is `build/daisi/src/main/Main`.
As shown, the environment for a simulation run can be selected with the `environment` parameter.
Additionally, a scenario has to be defined with the `scenario` parameter.
