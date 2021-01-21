#ifndef RLAPI_DHSETTING_H
#define RLAPI_DHSETTING_H

#include <gp_Trsf.hxx>
#include <QList>

#include "OCC/AIS_Coordinate.h"

class RLAPI_DHSetting
{
public:
    RLAPI_DHSetting();
    void Compute();
    QList<Handle(AIS_Coordinate)> GetCoords() {
        return DHCoords;
    }

private:
    QList<double> theta;
    QList<double> d;
    QList<double> alpha;
    QList<double> a;
    QList<gp_Trsf> DHTrsfs;
    QList<Handle(AIS_Coordinate)> DHCoords;
    gp_Pnt srcPnt;
};

#endif // RLAPI_DHSETTING_H
