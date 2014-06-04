CXX := $(CXX)
CXXFLAGS := $(CXXFLAGS) -DDEBUG -g -O0
LDFLAGS := $(LDFLAGS)
GDAL_CXXFLAGS := $(shell gdal-config --cflags)
GDAL_LDFLAGS := $(shell gdal-config --libs) $(shell gdal-config --dep-libs)

OS:=$(shell uname -s)
ifeq ($(OS),Darwin)
	SHARED_FLAG = -dynamiclib
	TEST_LIB = libtest.dylib
else
	SHARED_FLAG = -shared
	TEST_LIB = libtest.so
endif

all: $(TEST_LIB) gdal_test.input

./deps/libuv:
	git clone git://github.com/joyent/libuv.git ./deps/libuv
	(cd ./deps/libuv/ && ./autogen.sh && ./configure && make)

gdal_test.input:
	$(CXX) -o gdal_test.input plugin.cpp -fPIC $(GDAL_CXXFLAGS) $(GDAL_LDFLAGS) $(SHARED_FLAG)

$(TEST_LIB): gdal_test.input ./deps/libuv test_lib.cpp
	$(CXX) -o $(TEST_LIB) test_lib.cpp -I./ -fPIC $(CXXFLAGS) $(LDFLAGS) $(SHARED_FLAG)

run-test: $(TEST_LIB)
	$(CXX) -o run-test test.cpp -L./ -ltest $(CXXFLAGS) $(LDFLAGS) -Ideps/libuv/include deps/libuv/.libs/libuv.a

test: run-test
	./run-test

clean:
	rm -rf ./gdal_test.input ./run-test ./run-test.dSYM ./$(TEST_LIB) ./build

distclean:
	rm -rf deps

.PHONY: test
