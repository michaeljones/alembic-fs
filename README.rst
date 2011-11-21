alembic-fs
==========

This is an FUSE based filesystem view for Alembic files.

Getting Started
---------------

To build it, edit the Makefile to point at the appropriate locations on your
system and run::

   make

In the root directory. Apologies for not providing a portable Makefile, I've got
no experience with creating them.

Then run::

   abcd <alembic file>

And you will be placed in a new tcsh shell at the top level of the new Alembic
filesystem which has the following features:

- The object hierarchy is represented as a directory hierarchy.
- Object properties found in a directory called "properties" at the appropriate
  object level.
- Compound properties are representated as folders with subfolders for scalar
  and array properties.
- Scalar and array properties are represented as folders containing "Property
  Views" as files. There are the following property views:

  - values: Contains a space separated list of the values of that property.
  - max: Contains the single maximum value of the property.
  - min: Contains the single minimum value of the property.
  - json: Contains the values, max and min data marked-up as a json object.

Exit the shell, with "exit" or Ctrl-D, to leave and unmount the filesystem.

.. note:: Properties which have names starting with a "." will fall foul of the
   standard Unix convention which considers them hidden.

Example Session
---------------

::

   test1$ abcd Xform_tree.abc
   mounting file system...
   Xform_tree.abcd$ ls
   4_0  4_1  4_2  4_3
   Xform_tree.abcd$ cd 4_0/
   4_0$ ls
   3_0  3_1  3_2  3_3  properties
   4_0$ cd 3_3/
   3_3$ ls
   2_0  2_1  2_2  2_3  properties
   3_3$ cd properties/
   properties$ ls
   properties$ ll
   total 14M
   dr-------- 1 mike mike 4.0K 2011-11-19 16:53 .
   dr-------- 1 mike mike 4.0K 2011-11-19 16:53 ..
   dr-------- 1 mike mike 4.0K 2011-11-19 16:53 .xform
   properties$ cd .xform/
   .xform$ ll
   total 23M
   dr-------- 1 mike mike 4.0K 2011-11-19 16:53 .animChans
   dr-------- 1 mike mike 4.0K 2011-11-19 16:53 .inherits
   dr-------- 1 mike mike 4.0K 2011-11-19 16:53 isNotConstantIdentity
   dr-------- 1 mike mike 4.0K 2011-11-19 16:53 .ops
   dr-------- 1 mike mike 4.0K 2011-11-19 16:53 .vals
   .xform$ cd .ops/
   .ops$ ll
   total 19M
   -r-------- 1 mike mike 4.0K 2011-11-19 16:53 json
   -r-------- 1 mike mike 4.0K 2011-11-19 16:53 max
   -r-------- 1 mike mike 4.0K 2011-11-19 16:53 min
   -r-------- 1 mike mike 4.0K 2011-11-19 16:53 values
   .ops$ cat json 
   {
       "values" : [ 16, 32, 32, 32, 0 ],
       "max" : 32,
       "min" : 0
   }
   .ops$ cat max 
   32
   .ops$ cat min 
   0
   .ops$ cat values 
   16 32 32 32 0 
   .ops$ python
   Python 2.7.1+ (r271:86832, Apr 11 2011, 18:13:53) 
   [GCC 4.5.2] on linux2
   Type "help", "copyright", "credits" or "license" for more information.
   >>> import simplejson
   >>> simplejson.load(open("json"))
   {'max': 32, 'values': [16, 32, 32, 32, 0], 'min': 0}
   >>> 
   .ops$ exit
   test1$

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
- `fuse-examplefs <http://code.google.com/p/fuse-examplefs/>`_

