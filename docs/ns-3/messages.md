# Messages

A key part of a network simulation is exchanging messages.
Almost every component of SOLA uses messages for communication, e.g., [MINHTON](../minhton_lib/programmers/messages.md), [natter](../natter_lib/programmers/messages.md), and [Path Planning](../daisi_lib/applications/path-planning/index.md).

## CPPS Messages

The CPPS application makes use of messages as well.
Messages of the CPPS application are separated by ``|``.

Most message types are stating their type with an ``uint8_t`` at the beginning of a message.

### Negotiation Message Structure

CallForProposal (TO -> AMRs):
`1;connection;startX;startY;stopX;stopY`

AcceptProposal (TO -> AMR):
`2;connection;startX;startY;stopX;stopY`

Propose (AMRs -> TO):
`3;connection;cost`

### Message Types

Physical AMR to logical AMR:

- PublishService (0)
- SendOrderUpdate (3)
- SendPositionUpdate (2)

Logical AMR to physical AMR:

- ExecuteOrder (1)

From TO to AMR:

- RequestCost (1)
- UseService (2)

From AMR to TO:

- ResponseCost (3)
