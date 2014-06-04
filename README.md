## GDAL crash atexit

[![Build Status](https://secure.travis-ci.org/springmeyer/gdal-atexit-crash.png)](https://travis-ci.org/springmeyer/gdal-atexit-crash) (should be failing)

Reduced testcase for:

 - http://trac.osgeo.org/gdal/ticket/5509

## Depends

 - GDAL: gdal-config on PATH
 - make / autotools
 - libuv: no need for external install as libuv will be built on the fly

## Details

Versions of GDAL this testcase does crash with:

  - libgdal1h amd64 1.10.0-1~precise1
  - `brew install gdal` on OS X 10.9 which brings in gdal-1.11.0
  - gdal-1.11.0 built as minimal static lib by mapnik-packaging
  - gdal trunk built as minimal static lib by mapnik-packaging

Versions of GDAL this testcase does not crash with:

  - gdal-1.10.1 built as minimal static lib by mapnik-packaging

TODO: test gdal-1.10.1 from source on ubuntu precise

## Discussion

This testcase demonstrates a segfault that happens in a thread as it exits.

The testcase includes:

  - test.cpp - A command line program that runs a thread of work using libuv
  - libtest - A shared library that loads gdal_plugin and calls a symbol from it in the thread pool
  - gdal_plugin - A shared library that links to libgdal and reads a GeoTIFF

This design mirrors the situation of using Mapnik via Node.js (where the [original crash was encountered](https://github.com/mapnik/node-mapnik/issues/251)) because:

  - node uses `dlopen` to open c++ addons
  - in the case of node-mapnik, it links to the libmapnik shared library
  - the libmapnik shared library uses `dlopen` to open `gdal.input`
  - `gdal.input` links to the libgdal library

I've found that if GDAL is statically or dynamically linked into the main process or libmapnik then this crash does not happen, so the testcase, in order to prompt the crash makes sure to load using dlopen.

## Build

To build the testcase do:

    make

Then to run the crasher do:

    make test

The expected output is:

```sh
./run-test
make: *** [test] Segmentation fault: 11
```

Backtrace on OS X (that can be found at `~/Library/Logs/DiagnosticReports/`) looks like:

```
Thread 0:: Dispatch queue: com.apple.main-thread
0   libsystem_kernel.dylib          0x00007fff8e41da1a mach_msg_trap + 10
1   libsystem_kernel.dylib          0x00007fff8e41cd18 mach_msg + 64
2   libsystem_kernel.dylib          0x00007fff8e41850b semaphore_destroy + 91
3   libsystem_platform.dylib        0x00007fff84664428 _os_semaphore_dispose + 23
4   libsystem_pthread.dylib         0x00007fff8316587a pthread_join + 568
5   run-test                        0x00000001056db3fe uv_thread_join + 14
6   run-test                        0x00000001056dbadb cleanup + 139
7   dyld                            0x00007fff5fe42ef5 ImageLoaderMachO::doTermination(ImageLoader::LinkContext const&) + 215
8   dyld                            0x00007fff5fe33279 dyld::runTerminators(void*) + 69
9   libsystem_c.dylib               0x00007fff8602d7a1 __cxa_finalize + 177
10  libsystem_c.dylib               0x00007fff8602da4c exit + 22
11  libdyld.dylib                   0x00007fff8fbe2604 start + 8

Thread 1 Crashed:
0   ???                             0x000000010593fb90 0 + 4388551568
1   libsystem_pthread.dylib         0x00007fff8316554f _pthread_tsd_cleanup + 86
2   libsystem_pthread.dylib         0x00007fff83162479 _pthread_exit + 111
3   libsystem_pthread.dylib         0x00007fff831618a4 _pthread_body + 149
4   libsystem_pthread.dylib         0x00007fff8316172a _pthread_start + 137
5   libsystem_pthread.dylib         0x00007fff83165fc9 thread_start + 13
```