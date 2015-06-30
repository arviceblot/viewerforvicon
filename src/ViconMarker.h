#pragma once

#include <array>
#include <string>

namespace mmadlab
{
    class ViconMarker
    {
    public:
        ViconMarker();
        ~ViconMarker();

        const std::array<double, 3> getTranslation() const;
        void setTranslation(const double (&other)[3]);

        std::string name;
        std::string parentName;

    protected:
        std::array<double, 3> translation;
    };
}
