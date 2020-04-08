## Error handling

* Exceptions when thrown exit the program without calling guard destructors
to restore the terminal state, leaving the terminal corrupted.
* Throw exceptions with context variables, not just a constant string.
* When calling C functions that return int/errno capture the error string
and include it in the exception context.