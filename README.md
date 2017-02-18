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

Note: if making changes, use

    git update-index --assume-unchanged INSTALL

to avoid automatic changes to INSTALL making it into the Git history.
