## GDAL crash atexit

Reduced testcase for https://github.com/mapnik/node-mapnik/issues/251

## Depends

 - GDAL: gdal-config on PATH
 - make / autotools
 - libuv: no need for external install as libuv will be built on the fly

## Build

To build the testcase do:

    make

Then to run the crasher do:

    make test