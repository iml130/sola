# Logging

DAISI implements loggers for ns-3 simulations that create a single SQLite database.
Independent of the simulation environment, a global logger manager (defined in `daisi/src/logging/logger_manager.h`) is created at the start of the simulation.
The `LoggerManager` also provides methods for creating the specific loggers (MINHTON, natter, Autonomous Mobile Robot (AMR), Transport Order (TO), Path Planning, and SOLA).

Each specific logger includes `daisi/src/logging/definitions.h` where the functions are responsible for easily generating SQL `CREATE` and `INSERT` statements from structs.

`SQLiteHelper` is the class (defined in`daisi/src/logging/sqlite/sqlite_helper.h`) which has the logic for creating and modifying the SQLite database.

## Specify output path for simulation results

The database output of a scenario can be specified either by setting ``outputPath`` within the scenario file or by setting the ``DAISI_OUTPUT_PATH`` environment variable.
If both ways are used simultaneously, ``outputPath`` from the scenario file is used

## Deferred logging

CMake option to disable deferred logging: `-DDAISI_SQLITE_DEFERRED_LOGGING=OFF`
When deferred logging is disabled, all SQLite statements are executed immediately, impacting the performance negatively.
