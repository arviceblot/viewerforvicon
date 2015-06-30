#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ViconMarker.h"

namespace mmadlab
{
    class ViconSubject
    {
    public:
        ViconSubject();
        ~ViconSubject();

        // markers
        std::string name;
        unsigned int markerCount;
        std::vector<std::shared_ptr<ViconMarker>> markers;
    };
}
