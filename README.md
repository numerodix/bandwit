# Bandwidth monitor

`bandwit` is a terminal based bandwidth monitor that runs on Linux and BSD.

![bandwit](doc/bandwit.gif)


## Data sources

In Linux there are lots of places to get this information:

* `/sys/class/net/<iface>/statistics/{r,t}x_bytes` is the most convenient cause
  it's a file with literally just the number we want in it.
* `/proc/net/dev` requires finding the right line and parsing the right
  integers.
* `ip -statistics link show dev <iface>` requires finding the right lines and
  parsing the right integers.

In BSD there is only one way I know that doesn't require linking against system
libraries:

* `netstat -ibn` requires finding the right line and parsing the right
  integers.
