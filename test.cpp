#include <iostream>
#include <dlfcn.h>
#include <uv.h>

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

    void * handle = dlopen(SHARED_LIBRARY_NAME,RTLD_LAZY);
    baton_t *baton = new baton_t();
    if (handle) {
        load_func load = reinterpret_cast<load_func>(dlsym(handle, "load_plugin"));
        if (load) {
            load();
        } else {
            std::clog << "could not load 'load_plugin' symbol\n";            
        }
        baton->request.data = baton;
        baton->error = false;
        baton->result = 0;
        baton->callable = reinterpret_cast<run_func>(dlsym(handle, "do_work"));
        if (baton->callable) {
            uv_queue_work(uv_default_loop(), &baton->request, DoWork, (uv_after_work_cb)AfterWork);
        } else {
            std::clog << "could not load 'do_work' symbol\n";
        }
    } else {
        std::clog << "could not open libtest\n";
    }
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    if (handle) {
      dlclose(handle);
    } else {
      std::clog << "handle invalid\n";
    }
    return 0;
}