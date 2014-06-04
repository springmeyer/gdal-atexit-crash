#include <iostream>
#include <uv.h>
#include <cassert>
#include "test_lib.hpp"

typedef struct {
    uv_work_t request;
    int result;
    bool error;
} baton_t;

// Do something related to GDAL and Geotiff access in the threadpool
void DoWork(uv_work_t* req) {
    baton_t *baton = static_cast<baton_t *>(req->data);
    try
    {
        baton->result = testcase::do_work();
    }
    catch (std::exception const& ex)
    {
        baton->error = true;
    }
}

// After we return from the threadpool ensure we fetched the correct info
void AfterWork(uv_work_t* req) {
    baton_t *baton = static_cast<baton_t *>(req->data);
    // should be 3 bands reported
    assert(baton->result == 3);
    delete baton;
}

int main() {
    // load up the "gdal_test.input" plugin that links to libgdal
    assert(testcase::load_plugin() == true);
    // create a baton to pass into the libuv threadpool
    baton_t *baton = new baton_t();
    baton->request.data = baton;
    baton->error = false;
    baton->result = 0;
    // queue up callbacks
    // learn about libuv at http://nikhilm.github.io/uvbook/introduction.html
    uv_queue_work(uv_default_loop(), &baton->request, DoWork, (uv_after_work_cb)AfterWork);
    // now actually run pending callbacks
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    // when we get here it means that no more callbacks need to be processed
    assert(baton->error == false);
    return 0;
}