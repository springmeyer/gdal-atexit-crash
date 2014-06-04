## GDAL crash atexit

[![Build Status](https://secure.travis-ci.org/springmeyer/gdal-atexit-crash.png)](https://travis-ci.org/springmeyer/gdal-atexit-crash) (should be failing)

Reduced testcase for:

 - http://trac.osgeo.org/gdal/ticket/5509
 - https://github.com/mapnik/node-mapnik/issues/251

## Depends

 - GDAL: gdal-config on PATH
 - make / autotools
 - libuv: no need for external install as libuv will be built on the fly

## Discussion

This testcase includes a plugin that is loaded by a shared library which is loaded by a command line program.

This might seem odd, but it models the situation of using Mapnik via Node.js because:

  - node using `dlopen` to open c++ addons
  - in the case of node-mapnik, it links to the libmapnik shared library
  - the libmapnik shared library uses `dlopen` to open `gdal.input`
  - `gdal.input` links to the libgdal library

I've found that if GDAL is statically or dynamically linked into the main process then this crash does not happen, so the testcase, in order to prompt the crash makes sure to load using dlopen.

## Build

To build the testcase do:

    make

Then to run the crasher do:

    make test

The expected output is:

```sh
./run-test
result 3
unloading
dlclose: 0
make: *** [test] Segmentation fault
```