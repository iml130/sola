# SOLA


<!-- TODO: Figure of SOLA, what is it, deep dive, ... -->

SOLA uses the [management overlay MINHTON](../minhton_lib/index.md) and the [event dissemination natter](../natter_lib/index.md).

## Getting Started

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


## Academic Attribution

If you use it for research, please include one of the following references in any resulting publication.

```plain
@inproceedings{detznerSOLADecentralizedCommunication2023,
  title = {{{SOLA}}: {{A Decentralized Communication Middleware Developed}} with Ns-3},
  shorttitle = {{{SOLA}}},
  booktitle = {Proceedings of the 2023 {{Workshop}} on Ns-3},
  author = {Detzner, Peter and Gödeke, Jana and Tönning, Lars and Laskowski, Patrick and Hörstrup, Maximilian and Stolz, Oliver and Brehler, Marius and Kerner, Sören},
  date = {2023-06-28},
  series = {{{WNS3}} '23},
  pages = {78--85},
  publisher = {{Association for Computing Machinery}},
  location = {{New York, NY, USA}},
  url = {https://dl.acm.org/doi/10.1145/3592149.3592151},
  abstract = {The transformation from static production facilities into a flexible and decentralized cyber-physical production system (CPPS) is part of the current ongoing Industry 4.0. A CPPS will enable and support communication between people, machines and virtual objects, e.g., as material flow or products, alike. However, communication in CPPS relies often on centralized approaches using a message broker or is not considered at all. We present in this paper the decentralized communication middleware called SOLA with an emphasis on, but not limited to, CPPS. SOLA uses the inherent given capability to communicate of participating nodes and eliminates the need for a central instance. A structured overlay network is created and managed, which appears to its users as a single coherent system. The main building blocks of SOLA are the management overlay and the event dissemination. Within this building blocks, no single peer has a global view and all operations are based on each peer’s local view. The local view represents some selected links to a subset of all peers in the network. In addition to this, we also present how SOLA was developed with the help of the discrete-event simulator ns-3. Finally, we also show how we have used ns-3 to simulate a self-organizing material flow where participants use SOLA to communicate.},
  isbn = {9798400707476},
  keywords = {balanced tree,cyber-physical production system,decentral organized communication,peer-to-peer network}
}
```