#include <iostream>
#include <uv.h>
#include "test_lib.hpp"

typedef void (*load_func)();
typedef int (*run_func)();
typedef int (*unload_func)();

typedef struct {
    uv_work_t request;
    int result;
    bool error;
    std::string error_name;
    run_func callable;
} baton_t;

void DoWork(uv_work_t* req)
{
    baton_t *baton = static_cast<baton_t *>(req->data);
    try
    {
        baton->result = baton->callable();
    }
    catch (std::exception const& ex)
    {
        baton->error = true;
        baton->error_name = ex.what();
    }
}

void AfterWork(uv_work_t* req)
{
    baton_t *baton = static_cast<baton_t *>(req->data);
    std::clog << "result " << baton->result << "\n";
    delete baton;
}

int main(int argc, char *argv[]) {

    testcase::load_plugin();
    baton_t *baton = new baton_t();
    baton->request.data = baton;
    baton->error = false;
    baton->result = 0;
    baton->callable = &testcase::do_work;
    uv_queue_work(uv_default_loop(), &baton->request, DoWork, (uv_after_work_cb)AfterWork);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    return 0;
}