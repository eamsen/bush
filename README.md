# Bush - Strategic Voting
by Eugen Sawin <esawin@me73.com>

## License
The MIT License

## Requirements
* POSIX.1b-compliant operating system (librt)
* GNU GCC 4.6 or newer
* GNU Make
* Python 2.7 or newer (only for style checking)

## Dependencies
### Required
* gflags (http://code.google.com/p/gflags or `$ make gflags`)

### Optional
* gtest (http://code.google.com/p/googletest, only for testing)
* gperftools (http://code.google.com/p/gperftools, only for profiling)
* cpplint (`$ make cpplint`, only for style checking)

## Building gflags
The repository contains a slightly modified gflags version with less verbose
help output.  
If you want to use the provided version instead, you need to build gflags
locally:

    $ make gflags

and then activate the two lines in the makefile, which are commented out.
Alternatively you can build all dependencies at once:

    $ make depend

## Building Bush (depends on gflags)
To build Bush use:
    $ make  or  $ make bush

To build Nixon use:
    $ make nixon

To build Gandhi use:
    $ make gandhi

To build all of them at once use:
    $ make all

Bush (the strategy), Nixon and Gandhi are just different strategies of Bush 
(the program).
Alternatively you can just build Bush and use the `--strategy` flag to switch
between the strategies on startup.

## Using Bush
To start Bush use:

    $ bush <preferences.vote> <voter id> <voting system>

To show the full usage and flags help use:

    $ bush -help

## Testing Bush (depends on gtest)
To build and run the unit tests use:

    $ make check

## Profiling Bush (depends on gperftools)
To build Bush with profiling turned on use:

    $ make profile

## Getting cpplint
Code style checking depends on a modified version of Google's cpplint. Get it via
  
    $ make cpplint

## Checking style (depends on cpplint)
To test code style conformance with the [Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml) use:

    $ make checkstyle

