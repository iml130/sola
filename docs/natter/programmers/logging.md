# Logging

**natter** uses an easily extendable logging framework. It is possible to set a log-level and use multiple loggers at the same
time (e.g., for logging to console and to database).

## String Logging

**natter** can log strings with an appropriate log level. The available log levels in descending order are:

- **CRITICAL**
- **WARNING**
- **INFO**
- **DEBUG**

## Event Logging

Besides the logging of strings, events and event-data can be also logged.
The available event logging functions can be found in ``logging/logger.h``.

## Creating New Logger

Implement [``natter::logging::LoggerInterface``](https://iml130.github.io/sola/doxygen/classnatter_1_1logging_1_1LoggerInterface.html) and add the logger by calling [``Logger::get().addLogger()``](https://iml130.github.io/sola/doxygen/classnatter_1_1logging_1_1Logger.html#a8c45e558cee39ec936956b63c0383787).
