## Error handling

* Throw exceptions with context variables, not just a constant string.
* When calling C functions that return int/errno capture the error string
and include it in the exception context.
* Use smart pointers to track object lifetime.