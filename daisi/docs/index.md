# ns-3 Integration

- what is ns-3 
- why ns-3 (testing, verification, holistic simulation, ...)
- modular testing of
    - SOLA
    - MINHTON
    - NATTER
    - Applications



After you have built the simulation executable, you can run the DAISI simulation.
We assume that we are at the root level of the repository folder now.
`daisi_exec` is located there if you used the commands from the [installation guide for the terminal](../installation.md#terminal).
Otherwise, please replace `daisi_exec` with the respective simulation executable or simply run DAISI from VS Code.
A requirement is to set the environment variables, at least the `LD_LIBRARY_PATH`, while running cpps or sola (standalone) additionally require setting the `MINHTONDIR`:

```sh
export LD_LIBRARY_PATH=/work/ns3/ns3_installs/ns3_3_38_release_stripped/lib
export MINHTONDIR=`pwd`/minhton
```

Please make sure to adjust the specified `LD_LIBRARY_PATH` with your ns-3 installation location.

It is required to set an `environment` and a corresponding `scenario` as parameters like in the following example:

```sh
./daisi_exec --environment=cpps --scenario=daisi/scenarios/cpps/default.yml
```

You can replace the `scenario` option with `scenariostring` if you want to include the whole scenario content as an argument.
Another option is `disable-catch`, which disables catching fatal errors if set to `true`.

Examples for other environments than cpps can be found [on the environments page](environments.md).

# WHAT DOES IT INCLUDE
