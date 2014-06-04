#include <iostream>
#include <dlfcn.h>
#include <cstdlib>

typedef int (*work_func)();

static void * plugin_handle = NULL;

extern "C" void unload_plugin() {
    std::clog << "unloading\n";
    if (plugin_handle)
    {
        std::clog << "dlclose: " << dlclose(plugin_handle) << "\n";    
    }
}

extern "C" bool load_plugin() {
    plugin_handle = dlopen("./gdal_test.input",RTLD_LAZY);
    if (plugin_handle) {
        std::atexit(&unload_plugin);
        return true;
    } else {
        std::clog << "could load plugin\n";
        return false;
    }
}

extern "C" int do_work() {
    work_func callable = reinterpret_cast<work_func>(dlsym(plugin_handle, "do_work"));
    if (callable) {
        return callable();        
    } else {
        std::clog << "callable not valid\n";
        return 0;
    }
}
