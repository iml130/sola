# SolaNet

Basic asynchronous UDP networking library for use with MINHTON/natter/SOLA. May be replaced with something more advanced in the future.

## Dependencies

* [asio](https://think-async.com/Asio/) (tested with **boost-free** version only)
* pthreads

## Build

```shell
cd SolaNet
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage

Create an instance of ``solanet::Network`` by passing a callback function (the function which will be called
asynchronous when a message arrives).
That's all to set up the networking.

SolaNet will automatically select a free port. You can query the port by calling ``getPort()``.

To send data, call ``send(const Message &msg)`` function with a ``solanet::Message`` filled with
IP (v4 only) and port of the recipient and your message.

## (Currently) missing features

* Multicast support
* (Advanced) Package fragmentation
* (Advanced) Optional reliability flag
* (Advanced) Encryption
