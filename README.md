# rolo

[vCard](https://tools.ietf.org/html/rfc2426) viewer and editor.  Fork
of the [Rolo](http://rolo.sourceforge.net/) project by Andrew Hsu,
which seems abandoned.

Original read me file provided in README.

## Compilation

    autoreconf -f -i
    ./configure
    make
    make install

If shunit2 is installed on the system, then the [unit tests](test/run-tests)
will be exercised.
