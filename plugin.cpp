#include <gdal_priv.h>
#include <iostream>

/*
extern "C" __attribute__((constructor)) void init_it() {
    std::clog << "registering NOW\n";
    GDALAllRegister();  
}

extern "C" __attribute__((destructor)) void finish() {
    std::clog << "destroying NOW\n";
    GDALDestroyDriverManager();  
}
*/
extern "C" int do_work() {
    GDALAllRegister();  
    GDALDataset* dataset = (GDALDataset*)GDALOpen("./data/natural_earth.tif", GA_ReadOnly);
    if (dataset) {
        GDALClose(dataset);
        return dataset->GetRasterCount();
    } else {
        std::clog << "could not open tiff\n";
    }
    return 0;
}