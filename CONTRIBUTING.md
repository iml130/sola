# Contribute to SOLA

Here you find information on how to contribute to SOLA.

## Coding Style

With some exceptions listed below, we are following the [Google Coding Style Guide](https://google.github.io/styleguide/cppguide.html).

### SQL Style Guide

- Tables
	- General tables: UpperCamelCase, singular, e.g., `TransportOrder`
	- Views: lowerCamelCase beginning with "view", e.g., `viewFinishedTransportTraffic`
	- Enumerations (for listing types): lowerCamelCase beginning with "enum", e.g., `enumTransportOrderState`
- Columns
	- General columns: UpperCamelCase, e.g., `ModelName`
	- Columns with amounts/measurements: Append the unit at the end after an underscore, e.g., `Weight_kg`
		- For derived units consisting of multiple units, append them and use "p" for a division. Do not use numbers or special characters. E.g., meter per second squared (m/s^2) becomes `mpss`.
		- Some other common units: `us` for microseconds, `ms` for milliseconds, `ut` for unix timestamps
	- Foreign key: UpperCamelCase, is usually the referenced table name (or an abbreviation of it) followed by "Id" or "Uuid", e.g., `ApplicationId`

### Deviations and additonal aspects:
- **Comments/Comment Style**:
We are using Doxygen to automatically create documentation.
Use ``///`` to comment classes, methods and members.
For all other comments use ``//``.
- **Formatting**: We are using ``clang-format`` to format the code according to the ``.clang-format`` config.

## Commit Guidelines
- Use an expressive commit message. If possible, link the commit with an issue.
- Use commit messages according to the [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/) standard. Note the scope whenever possible.
  - Possible types: *fix*, *feat*, *build*, *ci*, *docs*, *style*, *refactor*, and *test*. 
  - Possible scopes are (but not limited to): *cpps*, *daisi*, *ns-3*, *path_planning*, *evaluation*, *logical_amr*, *physical_amr*, *amr*, *sola*, *minhton*, *natter*, *solanet*.
- We keep a linear git history. Use ``git rebase`` to keep your branch on top of the main branch.
- For a pull request to get merged, the code must adhere to the coding style guide, must be tested with test-cases (if required), the status checks must succeed and the documentation must be adjusted (if required). 
