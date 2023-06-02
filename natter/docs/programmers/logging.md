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

Besides the logging of strings, events and event-data can be also logged. The available events and logging functions can be found in ``logging/logger.h``.

## Creating New Logger

Implement ``natter::logging::LoggerInterface`` and add the logger by calling ``Logger::get().addLogger()``.
