# Event Types

MINHTON and natter differentiate between event types on their own.
When those types are being logged to the database, we add a constant to distinguish what component is responsible for which event.
The brackets show those event types with the added constant used for the logging to databases.

## MINHTON

Range: 0 - 255

- Join = 0 (0)
- Leave = 1 (1)
- Swap = 2 (2)
- Find Query = 3 (3)
- Request Countdown Start = 4 (4)

## natter

Range: 256 - 511

- Node Initialization (with Level and Number) = 1 (257)
- Publish = 2 (258)
