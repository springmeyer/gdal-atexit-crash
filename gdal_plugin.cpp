#include <gdal_priv.h>
#include <iostream>

extern "C" __attribute__((constructor)) void init_it() {
    GDALAllRegister();  
}

extern "C" __attribute__((destructor)) void finish() {
    GDALDestroyDriverManager();  
}

extern "C" int do_work() {
    //GDALAllRegister();  
    GDALDataset* dataset = (GDALDataset*)GDALOpen("./data/natural_earth.tif", GA_ReadOnly);
    if (dataset) {
        int raster_count = dataset->GetRasterCount();
        GDALClose(dataset);
        return raster_count;
    }
    return 0;
}