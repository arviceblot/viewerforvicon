#include "ViconMarker.h"

namespace mmadlab
{
    ViconMarker::ViconMarker()
    {
        this->name = "";
        this->parentName = "";
        this->translation = {0, 0, 0};
    }

    ViconMarker::~ViconMarker()
    {
    }

    const std::array<double, 3> ViconMarker::getTranslation() const
    {
        return this->translation;
    }

    void ViconMarker::setTranslation(const double (&other)[3])
    {
        this->translation = {other[0] / 10.0, other[1] / 10.0, other[2] / 10.0};
    }
}
