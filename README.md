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
  * gtest (not included, only required for testing)
  * gperftools (not included, only required for profiling)
  * gflags (included)
  * cpplint (included, only required for style checking)

## Building gflags
  Before building Bush, you need to build gflags locally once:
  `$ make gflags`

  Alternatively you can build all dependencies at once:
  `$ make depend`

## Building Bush (depends on gflags)
  To build Bush use:
  `$ make`  or  `$ make bush`

  To build Nixon use:
  `$ make nixon`

  To build Gandhi use:
  `$ make gandhi`

  To build all of them at once use:
  `$ make all`

  Bush, Nixon and Gandhi are just different strategies of Bush.
  Alternatively you can just build Bush and use the `--strategy` flag to switch
  between the strategies on startup.

## Using Bush
  `$ bush <preferences.vote> <voter id> <voting system>`

  To show the full usage and flags help use:
  `$ bush -help`

## Testing Bush (depends on gtest)
  To build and run the unit tests use:
  `$ make test`

## Profiling Bush (depends on gperftools)
  To build Bush with profiling turned on use:
  `$ make profile`

## Checking style
  To test code style conformance with the [Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml) use:
  `$ make checkstyle`
