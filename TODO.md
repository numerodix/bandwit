## Error handling

* Throw exceptions with context variables, not just a constant string.
* When calling C functions that return int/errno capture the error string
and include it in the exception context.

## State management

* Make sure terminal driver puts stdin into blocking mode before reading cursor
position - make this a guard.
* Make sure terminal mode setter also sets the modes necessary to make
reading the cursor position work - even if this is implied in the default
mode of common terminals.

## Object lifetime

* Use smart pointers to track object lifetime.

## UX

* To improve responsiveness: After a resize, notify the bar chart to repaint right away, without waiting for a second to elapse.