#pragma once

#include <memory>
#include <string>

#include "BaseApplication.h"
#include "Client.h"
#include "ViconSubject.h"

using namespace Ogre;
using namespace ViconDataStreamSDK::CPP;

namespace mmadlab
{
    class ViewerApplication : public BaseApplication
    {
    public:
        ViewerApplication(std::string hostname = "localhost");
        virtual ~ViewerApplication();

    protected:
        virtual void createCamera();
        virtual void createViewports();
        virtual void createScene();
        virtual bool frameRenderingQueued(const FrameEvent&);

        void updateMarkers();
        void updateEntities();
        void createSphere(const std::string&, const float, const int nRings = 16, const int nSegments = 16);


        Client client;
        std::vector<std::shared_ptr<ViconSubject>> subjects;
    };
}
