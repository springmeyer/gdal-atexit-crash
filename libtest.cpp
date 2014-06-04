#include <iostream>
#include <dlfcn.h>
#include <cstdlib>
#include <cassert>

namespace testcase {

typedef int (*work_func)();

static void * plugin_handle = NULL;

void unload_plugin() {
    assert(plugin_handle);
    assert(dlclose(plugin_handle) == 0);
}

bool load_plugin() {
    plugin_handle = dlopen("./gdal_plugin.input",RTLD_LAZY);
    if (plugin_handle) {
        // NOTE: commenting this std::atexit avoids crash
        // since the dlclose is required to trigger it
        std::atexit(&unload_plugin);
        return true;
    } else {
        return false;
    }
}

int do_work() {
    work_func callable = reinterpret_cast<work_func>(dlsym(plugin_handle, "do_work"));
    if (callable) {
        return callable();        
    } else {
        return 0;
    }
}

}
