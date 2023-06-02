# Main Features

SOLA is

- **scalable**
    - Efficient algorithms for maintaining a structured overlay network and event dissemination allow for a huge number of nodes.
- **fast**
    - Implemented completely in C++.
    - Exchanges messages in binary format for high throughput.
- **resilient**
    - No single node is responsible for the message dissemination, thus, even when a single node fails the message dissemination will continue.
- **tested**
    - Many unit and integration tests run on every change in the continuous integration pipeline.
- **modular**
    - The implementations of many parts can be exchanged at any time due to the usage of interfaces.
- **versatile**
    - Multiple loggers can be defined.
    - Many implementations are based on interfaces enabling replacing the algorithms easily.
