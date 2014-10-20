# Hacking Gladius

## Naming Conventions

###  Field Naming Conventions
- Non-public, non-static field names start with m.
- Static field names start with s.
- Other fields start with a lower case letter.

## Valgrid
```bash
valgrind --leak-check=full --trace-children=yes --track-origins=yes \
--log-file=OUT.gladius
```
