!!! danger "This is work in progress!"

First you need to build SOLA with the ns-3 integration as described in [Getting Started](../getting_started.md).
For each component/application, a single ns-3 simulation executable is created.
Following the building instructions, the executables are located at:
- [``minhton``](../minhton/introduction.md): ``<SOLA_REPOSITORY_DIR>/build/src/minhton-ns3/minhton_ns3``.
- [``natter``](../natter/introduction.md): ``<SOLA_REPOSITORY_DIR>/build/src/natter-ns3/natter_ns3``
- [``sola``](../index.md): ``<SOLA_REPOSITORY_DIR>/build/src/sola-ns3/SOLA_ns3``
- [``cpps`` (OptiMaFlow)](../optimaflow/introduction.md): ``<SOLA_REPOSITORY_DIR>/build/src/cpps-ns3/CPPS_ns3``

### Specify logging output path

Before running the simulation, the path where the SQLite database file should be created must be specified.
This database is used for logging simulation events, states and other simulation relevant data.
To specify the path, set the environment variable ``DAISI_OUTPUT_PATH`` to the desired output path, like ``export DAISI_OUTPUT_PATH=<PATH>``.


### Application-specific configurations

!!! warning "Running OptiMaFlow"

    To run OptiMaFlow, the ``MINHTON_DIR`` environment variable needs to be set.
    It should contain the path of ``<SOLA_REPOSITORY_DIR>/minhton``.
    This path is used to read MINHTON config files.

### Specify the scenariofile

The simulation is controlled by a YAML-based scenariofile.
Default scenariofiles for all applications are provided in the ``<SOLA_REPOSITORY_DIR>/daisi/scenarios`` folder.
The structure of the scenariofile is described in the "Scenariofile" section of each application's ns-3 integration documentation (see left sidebar).

The path to the scenariofile must be passed to the executable with ``--scenario=<SCENARIO_FILE_PATH>``.
This path must either be a absolute path or a path relative to the current working directory.

### Run the simulation

For example to start a OptiMaFlow simulation with your current working directory being ``<SOLA_REPOSITORY_DIR>``, run:

```build/src/cpps/CPPS_ns3 --scenario=daisi/scenarios/cpps/default.yml```

To start a different application, just use a different executable and a different scenariofile.

### Inspecting the output

While running the simulation, a SQLite database file is created at ``DAISI_OUTPUT_PATH``.
After the simulation finishes, you can inspect the database to get insights of the application behavior.

!!! danger "Wait until the simulation finishes!"

    Do not open this SQLite database file while the simulation is running!
    Otherwise the database might become corrupted!

Information about which data is logged to the database can be found under the "Logging" section of each application's ns-3 integration documentation (see left sidebar).
