# MINHTON

MINHTON is a peer-2-peer stack and library as a one-stop-shop for other tools to use.

We have spent long time to create a peer-2-peer library dealing with the whole stuff like joining, leaving and searching within the network. Building large scale peer-to-peer systems has been complex and difficult in the last decades, and libp2p is a way to fix that. We try to avoid as much external libraries as possible. MINHTON is built for cyber-physical production system but actually without the focus on one single domain so that a lots of people can use it, for different domains/ projects and applications.

You will find here our documentation, examples, tutorials and many other things to boost your existing and new projects and to avoid the single-source-of-failure, a centralized server ;-)

## Why you should use it

The question is why not ;)

## Installation

Please refer to the full documentation for installation details.

### Dependencies

Currently we only have a single dependency: `libyaml`. Make sure to have it installed on the system (via `apt`, `dpkg`, `pacman` or from sources!)

## Unit-Testing with CMake and Catch2

Enable CMake option ``MINHTON_ENABLE_TESTS`` to build the tests.
