# rolo

[vCard](https://tools.ietf.org/html/rfc2426) viewer and editor.  Fork
of the [Rolo](http://rolo.sourceforge.net/) project by Andrew Hsu,
which seems abandoned.

Original read me file provided in README.

You may also be interested in [khard](https://github.com/scheibler/khard),
an "address book for the Linux console" which can read and write vCard
files stored locally.

## Dependencies

Compilation of rolo requires some external libraries. In Debian-based
systems, the following packages are needed:

- libglib2.0-dev
- libncurses-dev
- libunac1-dev
- libvc-dev

## Compilation

    autoreconf -f -i
    ./configure
    make
    make install

If shunit2 is installed on the system, then the [unit tests](test/run-tests)
will be exercised.
