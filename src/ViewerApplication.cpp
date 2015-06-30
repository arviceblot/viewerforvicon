#include "ViewerApplication.h"

#include <iostream>

namespace mmadlab
{
    ViewerApplication::ViewerApplication(std::string hostname)
    {
        hostname += ":801";
        std::cout << "Connecting to " << hostname << std::endl;
        if (!client.Connect(hostname).Result == Result::Success)
        {
            std::cout << "Failed." << std::endl;
        }
        std::cout << "Connected." << std::endl;

        client.EnableMarkerData();

        client.SetStreamMode(ViconDataStreamSDK::CPP::StreamMode::ServerPush);

        // Ogre3D is Y-up, so let's change the client mapping
        client.SetAxisMapping(Direction::Forward, Direction::Up, Direction::Right);

        // Discover the version number
        Output_GetVersion outputVersion = client.GetVersion();
        std::cout << "Version: " << outputVersion.Major << "."
            << outputVersion.Minor << "."
            << outputVersion.Point << std::endl;

        // get frame before we start listing data
        client.GetFrame();
        client.GetFrame();

        // get subjects
        unsigned int subjectCount = client.GetSubjectCount().SubjectCount;
        std::cout << "Subjects (" << subjectCount << "):" << std::endl;
        for (unsigned int subjectIndex = 0; subjectIndex < subjectCount; subjectIndex++)
        {
            std::cout << "\tSubject #" << subjectIndex << std::endl;

            std::shared_ptr<ViconSubject> subject(new ViconSubject);

            // get subject name
            std::string subjectName = client.GetSubjectName(subjectIndex).SubjectName;
            std::cout << "\t\tName: " << subjectName << std::endl;
            subject->name = subjectName;

            // get marker cout
            unsigned int markerCount = client.GetMarkerCount(subjectName).MarkerCount;
            std::cout << "\t\tMarkers (" << markerCount << "):" << std::endl;
            subject->markerCount = markerCount;
            for (unsigned int markerIndex = 0; markerIndex < markerCount; markerIndex++)
            {
                std::shared_ptr<ViconMarker> marker(new ViconMarker);

                // get marker name
                std::string markerName = client.GetMarkerName(subjectName, markerIndex).MarkerName;
                marker->name = markerName;

                // get marker global translation
                Output_GetMarkerGlobalTranslation markerTranslation = client.GetMarkerGlobalTranslation(subjectName, markerName);
                std::cout << "\t\t\tMarker #" << markerIndex << ":  "
                    << markerName
                    << "\t(" << markerTranslation.Translation[0] << ", "
                    << markerTranslation.Translation[1] << ", "
                    << markerTranslation.Translation[2] << ")" << std::endl;
                marker->setTranslation(markerTranslation.Translation);

                subject->markers.push_back(marker);
            }

            subjects.push_back(subject);
        }
    }

    ViewerApplication::~ViewerApplication()
    {
        std::cout << "Disconnecting..." << std::endl;
        client.Disconnect();
    }

    void ViewerApplication::createCamera()
    {
        mCamera = mSceneMgr->createCamera("PlayerCam");
        mCamera->setPosition(Vector3(0, 300, 500));
        mCamera->lookAt(Vector3(0, 0, 0));
        mCamera->setNearClipDistance(5);

        mCameraMan = new OgreBites::SdkCameraMan(mCamera);
    }

    void ViewerApplication::createViewports()
    {
        Viewport* vp = mWindow->addViewport(mCamera);
        vp->setBackgroundColour(ColourValue(0, 0, 0));
        mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
    }

    void ViewerApplication::createScene()
    {
        // create the scene here
        // ambient light
        mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

        // a light
        Light* light = mSceneMgr->createLight("MainLight");
        light->setPosition(20, 80, 50);

        // ground plane
        Plane plane(Vector3::UNIT_Y, 0);
        MeshManager::getSingleton().createPlane(
                "ground",
                ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                plane,
                1500, 1500, 20, 20,
                true,
                1, 5, 5,
                Vector3::UNIT_Z);
        Entity* groundEntity = mSceneMgr->createEntity("ground");
        mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
        groundEntity->setCastShadows(false);
        groundEntity->setMaterialName("Ground");

        // create a new node for every marker for every subject?
        // each marker entity should take the naming convention of 'SubjectName-MarkerName'
        // consider making marker radius an app argument
        createSphere("marker.mesh", 2);
        for (auto subject: subjects)
        {
            for (auto marker: subject->markers)
            {
                Entity* markerEntity = mSceneMgr->createEntity(subject->name + "-" + marker->name, "marker.mesh");
                mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(markerEntity);
                markerEntity->setMaterialName("Marker");
            }
        }
    }

    bool ViewerApplication::frameRenderingQueued(const FrameEvent& frameEvent)
    {
        bool ret = BaseApplication::frameRenderingQueued(frameEvent);

        // handle input

        // update everything
        updateMarkers();
        updateEntities();

        return ret;
    }

    void ViewerApplication::updateMarkers()
    {
        // get the new frame
        client.GetFrame();
        // update the markers for each subject
        for (auto subject: this->subjects)
        {
            for (auto marker: subject->markers)
            {
                // update the marker translation
                Output_GetMarkerGlobalTranslation markerTranslation = client.GetMarkerGlobalTranslation(subject->name, marker->name);
                marker->setTranslation(markerTranslation.Translation);
            }
        }
    }

    void ViewerApplication::updateEntities()
    {
        // update each marker entity
        // this should only have an effect after updateMarkers() has been called
        for (auto subject: subjects)
        {
            for (auto marker: subject->markers)
            {
                auto markerNode = mSceneMgr->getEntity(subject->name + "-" + marker->name)->getParentSceneNode();
                auto translation = marker->getTranslation();
                markerNode->setPosition(translation[0], translation[1], translation[2]);
            }
        }
    }

    /* http://www.ogre3d.org/tikiwiki/tiki-index.php?page=manualspheremeshes#Creating_a_sphere_with_ManualObject */
    void ViewerApplication::createSphere(const std::string& strName, const float r, const int nRings, const int nSegments)
    {
        ManualObject* manual = mSceneMgr->createManualObject(strName);
        manual->begin("BaseWhiteNoLighting", RenderOperation::OT_TRIANGLE_LIST);

        float fDeltaRingAngle = (Math::PI / nRings);
        float fDeltaSegAngle = (2 * Math::PI / nSegments);
        unsigned short wVerticeIndex = 0 ;

        // Generate the group of rings for the sphere
        for (int ring = 0; ring <= nRings; ring++)
        {
            float r0 = r * sinf(ring * fDeltaRingAngle);
            float y0 = r * cosf(ring * fDeltaRingAngle);

            // Generate the group of segments for the current ring
            for (int seg = 0; seg <= nSegments; seg++)
            {
                float x0 = r0 * sinf(seg * fDeltaSegAngle);
                float z0 = r0 * cosf(seg * fDeltaSegAngle);

                // Add one vertex to the strip which makes up the sphere
                manual->position(x0, y0, z0);
                manual->normal(Vector3(x0, y0, z0).normalisedCopy());
                manual->textureCoord((float) seg / (float) nSegments, (float) ring / (float) nRings);

                if (ring != nRings)
                {
                    // each vertex (except the last) has six indicies pointing to it
                    manual->index(wVerticeIndex + nSegments + 1);
                    manual->index(wVerticeIndex);
                    manual->index(wVerticeIndex + nSegments);
                    manual->index(wVerticeIndex + nSegments + 1);
                    manual->index(wVerticeIndex + 1);
                    manual->index(wVerticeIndex);
                    wVerticeIndex++;
                }
            }; // end for seg
        } // end for ring
        manual->end();
        MeshPtr mesh = manual->convertToMesh(strName);
        mesh->_setBounds(AxisAlignedBox(Vector3(-r, -r, -r), Vector3(r, r, r)), false);

        mesh->_setBoundingSphereRadius(r);
        unsigned short src, dest;
        if (!mesh->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
        {
            mesh->buildTangentVectors(VES_TANGENT, src, dest);
        }
    }
}
