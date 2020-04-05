## Error handling

* Throw exceptions with context variables, not just a constant string.
* When calling C functions that return int/errno capture the error string
and include it in the exception context.
* When using `tcsetattr` check that all values were set, not just some.