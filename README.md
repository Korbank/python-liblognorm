liblognorm Python bindings
==========================

[Liblognorm](http://www.liblognorm.com/) is a log normalization library
capable of real-time processing. It can convert logs from flat string-blob
format to one of several standard formats (e.g. [CEE](https://cee.mitre.org/)
or JSON).

This is a Python bindings for liblognorm 1.1.x.

Usage
-----

    import liblognorm
    import sys
    import json

    print liblognorm.version() # version liblognorm (not the binding module)

    rulebase = "parsing.rules"
    lognorm = liblognorm.Lognorm(rulebase)
    log_line = sys.stdin.readline()
    event = lognorm.normalize(log_line.rstrip("\n"))
    print json.dumps(event)


Contact and License
-------------------

python-liblognorm is written by Stanislaw klekot <dozzie at jarowit.net> for
Korbank S.A <http://korbank.com/>.
The primary distribution point is <https://github.com/korbank/python-liblognorm>.

python-liblognorm is distributed under 3-clause BSD license. See COPYING file
for details.
