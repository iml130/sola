# Message types

## Serializer

``serializer.h`` declares a template class for serialization. This takes a type ``T`` containing the message and
returns it as a serialized ``std::string`` and vice versa.

Currently, the serializer uses [cereal](https://github.com/USCiLab/cereal.git). cereal supports
serialization into binary, JSON and XML.
