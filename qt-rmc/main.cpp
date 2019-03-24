
#include "CApplication.h"

// To check for memory leaks with VLD
// #include "VLD.h"

int main(int argc, char *argv[])
{
    // VLDEnable();

    CApplication app(argc, argv);

    if (app.init(argc, argv) == false)
        return -1;

    return app.exec();
}
