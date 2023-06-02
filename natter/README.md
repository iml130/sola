<center>
<img src="docs/img/logo.png" alt="natter logo" width="200" />
</center>

**natter** is a fully broker-less, decentralized topic-based publish/subscribe library.

Using this library, the following features are summarized:

- Nodes can subscribe to a specific topic and receive messages
- Publish a (broadcast) message to a topic using point-to-point connections, e.g., UDP 
- Each node has a low-memory overhead maintaining connection links to a few other nodes
- Supporting up to millions of nodes - in theory ;-)
 
# Design Goals

We address the following design goals:

- **Scalability**: Since each node maintains only a few links to other nodes, **natter** can handle a huge number of nodes
- **Efficiency**: Sending redundant messages is reduced to a minimum

# Usage
This is a quick guide to get **natter** up and running in a matter of minutes. 
The prerequisites for running **natter** are listed below.

## Required tools and dependencies

- A modern C++17 compliant compiler. Older versions won't work.
- CMake (>= 3.13.4)
- libuuid
- pthreads
- [libevent](https://libevent.org/)

## API

- [include/natter/natter.h](https://github.com/iml130/sola/tree/main/natter/include/natter/natter.h)
- [Doxygen](doxygen/index.html) # TODO: Final link


## Examples

We have implemented a simple [chat](/examples/chat/README.md) application that illustrates easy usage and power of natter.

# Community

If you want to get involved, feel free to ask questions, bring ideas that can be discussed, or anything else. We are open-source and we like open thoughts!


# DISCLAIMER
This is a research project and not intended for everyday use. The code is made available without any support. However, we welcome any kind of feedback via the issue tracker.
