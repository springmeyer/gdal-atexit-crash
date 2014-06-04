CXX := $(CXX)
CXXFLAGS := $(CXXFLAGS) -DDEBUG -g -O0
LDFLAGS := $(LDFLAGS)
GDAL_CXXFLAGS := $(shell gdal-config --cflags)
GDAL_LDFLAGS := $(shell gdal-config --libs)

OS:=$(shell uname -s)
ifeq ($(OS),Darwin)
	SHARED_FLAG = -dynamiclib
	SHARED_LIBRARY_NAME = ./libtest.dylib
	GDAL_LDFLAGS += $(shell gdal-config --dep-libs)
else
	SHARED_FLAG = -shared
	SHARED_LIBRARY_NAME = ./libtest.so
	LDFLAGS += -ldl -lpthread -lrt
endif

all: $(SHARED_LIBRARY_NAME) gdal_test.input

./deps/libuv:
	git clone git://github.com/joyent/libuv.git ./deps/libuv
	(cd ./deps/libuv/ && ./autogen.sh && ./configure && make)

gdal_test.input:
	$(CXX) -o gdal_test.input plugin.cpp -fPIC $(GDAL_CXXFLAGS) $(GDAL_LDFLAGS) $(SHARED_FLAG)

$(SHARED_LIBRARY_NAME): gdal_test.input ./deps/libuv test_lib.cpp
	$(CXX) -o $(SHARED_LIBRARY_NAME) test_lib.cpp -I./ -fPIC $(CXXFLAGS) $(LDFLAGS) $(SHARED_FLAG)

run-test: $(SHARED_LIBRARY_NAME)
	$(CXX) -o run-test test.cpp -DSHARED_LIBRARY_NAME=\"$(SHARED_LIBRARY_NAME)\" -L./ -ltest $(CXXFLAGS) -Ideps/libuv/include deps/libuv/.libs/libuv.a $(LDFLAGS)

test: run-test
	./run-test

clean:
	rm -rf ./gdal_test.input ./libtest* ./run-test ./run-test.dSYM $(SHARED_LIBRARY_NAME) ./build

distclean: clean
	rm -rf deps

.PHONY: test
