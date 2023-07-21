# Messages

Each message class is first and foremost made up of a header class. The header class contains a message type, target node, and sender node. The sender node is always the node that is sending exactly this message. The target node is always the node the message is directly sent to. In case the tree position of the target is not known, the network information must be set.

Additionally, the message header contains an event id and ref event id to identify events the message is initiating, respectively events the message is referring to. E.g., a join message is initiating a join procedure event and all originating update messages of that join procedure contain the join message's event id as their ref event id.

## Message Types

Please take a look at [the MessageType enum](https://iml130.github.io/sola/doxygen/namespaceminhton.html#enum-members) and [the MessageVariant and MessageSEVariant definitions](https://iml130.github.io/sola/doxygen/namespaceminhton.html#typedef-members) for more information.
The MessageType enum simply maps each message type to a number, where related message types have numbers closeby.
The MessageVariant and MessageSEVariant definitions include every message type class respectively every message type class that can be used in a Search Exact query.

## Adding a new message type

What needs to be done for adding a new message type:

- Create a new header file in `include/minhton/message` (Tip: Use join.h as a guideline)
    - Make sure to use a unique name for the ifndef include guide
    - Make sure to include `#include "minhton/message/message.h"` and `#include "minhton/message/serialize.h"`
    - Place a `SERIALIZE(...)` with required attributes under public visibility (Tip: Look at `include/minhton/algorithms/esearch/find_query.h` if the message type needs separate load and save functions)
    - Apart from the constructor for general usage which should have all parameters to create a valid message, a default constructor is required for serialization purposes.
    - Always required and the same are the `protected` fields that all message types must have.
- Create a corresponding new source file in `src/message`
- Include the message type header in `include/minhton/message/types_all.h` and extend the variant in alphabetical order
- Do the same in `include/minhton/message/se_types.h` for message types that can be used as a Search Exact query
- Include the source file in `src/message/CMakeLists.txt`
- Add an enum value in `include/minhton/message/types.h`
- Add the enum value to the correct interface header file in `include/minhton/algorithms/`
- Implement the message processing in the correct algorithm
- Add the test cases under `tests/`
- Adjust the state machine in `src/utils/fsm.cpp` and `include/minhton/utils/fsm.h`

## Using the message types

1. Include the relevant message type, e.g.: `#!cpp #include "minhton/message/join.h"`.
2. Create a MinhtonMessageHeader with the sender and target (of type `NodeInfo`) and optionally the ref event id if there is an existing event we want to reference: `#!cpp MinhtonMessageHeader header(getRoutingInfo()->getSelfNodeInfo(), target_node_info, ref_event_id);`.
3. Create the specific message with the header as the first parameter, followed by the parameters for the message type (**Do not use the default constructor except for test cases!**) like this: `#!cpp MessageJoin msg(header, getRoutingInfo()->getSelfNodeInfo());`.
4. Send the message with `#!cpp this->send(msg);`
5. Then the message is sent over the network using the NetworkFacade.
6. Finally, the message arrives at the target, where it is processed by the specified algorithm or handled by the `node.cpp`.
