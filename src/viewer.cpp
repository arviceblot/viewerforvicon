#include <iostream>
#include <string>

#include "Client.h"

#include "ViconMarker.h"
#include "ViconSubject.h"
#include "ViewerApplication.h"

using namespace mmadlab;

int main(int argc, char* argv[])
{
    // let's do a horrible thing an assume the first and only option
    // is what we're looking for
    std::string hostname = argv[1];
    ViewerApplication app(hostname);
    try
    {
        app.go();
    }
    catch (Ogre::Exception& e)
    {
        std::cerr << "Exception: " << e.getFullDescription().c_str() << std::endl;
    }
    return 0;
}
