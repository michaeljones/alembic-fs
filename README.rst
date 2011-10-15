alembic-fs
==========

This is an FUSE based filesystem view for Alembic files.

Getting Started
---------------

To build it, run::

   make

In the root directory.

Then run::

   abcd <alembic file>

And you will be placed in a new shell at the top level of the new Alembic
filesystem which has the following features:

- The object hierarchy is represented as a directory hierarchy.
- Object properties found in a directory called "properties" at the appropriate
  object level.
- Compound properties are representated as folders.
- Scalar and array properties are represented as files containing appropriate
  space separated values. Note that property which have names starting with a
  "." will fall foul of the standard Unix convention which considers them
  hidden.

Exit the shell, with "exit" or Ctrl-D, to leave and unmount the filesystem.

Requirements
------------

Development is currently done with:

- Fuse 2.8.5
- Alembic 1.0.2

Alembic requires:

- Boost 1.44
- hdf5 1.8.7
- ilmbase 1.0.1

Credits
-------

Thank you to:

- `The Alembic team <http://alembic.io>`_

