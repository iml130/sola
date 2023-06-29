# Network

## General notes

* Every switch can handle up to ``kNumberOfAppNodesPerSwitch`` nodes.
* Every WLAN AP can handle up to ``kMaxNumberAmrsPerAp`` nodes.
* The number of switches and routers are automatically scaled up with more nodes.

## Network structure (Logical AGV connected via Wireless)

```txt
[TO] --|                                            ))  [Physical and logical AGV]
       | -- [Switch] -- [Core Router] -- [WLAN AP] )))
[TO] --|                                            ))  [Physical and logical AGV]
```

* The ``[Physical and logical AGV]`` node is running two ns3 applications:
    * Application 0: Logical AGV using TCP 127.0.0.1:3000 and UDP for communication to other nodes.
    * Application 1: Physical AGV using TCP 127.0.0.1:4000

## IP ranges

* Core routers IP always ends with ``.1``

* TO/L-AGVs connected via switch to core router:
    * 1.1.0.0/16
    * 1.2.0.0/16
    * 1.3.0.0/16
    * ...
* Wireless network
    * 192.168.0.0/16
    * 192.169.0.0/16
    * 192.170.0.0/16
    * ...
