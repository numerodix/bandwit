## Error handling

* Throw exceptions with context variables, not just a constant string.
* When calling C functions that return int/errno capture the error string
and include it in the exception context.

## Object lifetime

* Use smart pointers to track object lifetime.

## STL

* Improve the grizzly regular expressions.
* Use emplace over push where possible.

## UX

* To improve responsiveness: After a resize, notify the bar chart to repaint
  right away, without waiting for a second to elapse.
