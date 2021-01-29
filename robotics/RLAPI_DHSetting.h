#ifndef RLAPI_DHSETTING_H
#define RLAPI_DHSETTING_H

#include <gp_Trsf.hxx>
#include <QList>

#include "OCC/AIS_Coordinate.h"

#include "robotics/RLAPI_Writer.h"

class RLAPI_DHSetting
{
public:
    //! constructor
    RLAPI_DHSetting();

    //! destructor
    ~RLAPI_DHSetting();

    //! compute the coordinates of DH with the arguments of
    //! it. The calculation formula is
    //!
    //!        |cosθi -sinθi•cosαi  sinθi•sinαi ai•cosθi|
    //! |Ai| = |sinθi  cosθi•cosαi -cosθi•sinαi ai•sinθi|
    //!        |  0       sinαi        cosαi       di   |
    //!        |  0          0           0          1   |
    //!
    //! |X|       |0|
    //! |Y| = ∏Ai•|0|
    //! |Z|       |0|
    //! |1|       |1|
    //!
    //! the gp_Trsf class has hide the row (0,0,0,1), but
    //! use it in default when calculating, and the
    //! gp_Trsf::TranslationPart is the coordinate value
    //!
    void Compute();

    //! compute the FK, for displaying the coordinates if joint
    //! has rotation value
    void ComputeFK();

    //! apply the DH arguments and write these arguments to
    //! the mdl xml file
    void ApplyDHArgs(const QString& mdlFileName);

    //! get the cooordinates' shapes for displaying
    QList<Handle(AIS_Coordinate)> GetCoords() const {
        return DHCoords;
    }

    //! set the coordinates' origin position
    void SetBasePosition(const gp_Pnt &aPnt) {
        srcPnt = aPnt;
    }

    //! set the value of rotation of each joint
    void SetJointDelta(const QList<double> &vals) {
        delta = vals;
    }

    //! set the θ DH arg, this function should use before compute
    void SetTheta(const QList<double> &vals) {
        theta = vals;
    }

    //! set the d DH arg, this function should use before compute
    void SetD(const QList<double> &vals) {
        d = vals;
    }

    //! set the α DH arg, this function should use before compute
    void SetAlpha(const QList<double> &vals) {
        alpha = vals;
    }

    //! set the a DH arg, this function should use before compute
    void SetA(const QList<double> &vals) {
        a = vals;
    }

    //! return the theta value
    QList<double> GetTheta() const {
        return theta;
    }

    //! return the d value
    QList<double> GetD() const {
        return d;
    }

    //! return the alpha value
    QList<double> GetAlpha() const {
        return alpha;
    }

    //! return the a value
    QList<double> GetA() const {
        return a;
    }

private:
    QList<double> theta;
    QList<double> d;
    QList<double> alpha;
    QList<double> a;
    QList<double> delta;
    QList<gp_Trsf> DHTrsfs;
    QList<double>  mdlArgs;
    QList<Handle(AIS_Coordinate)> DHCoords;
    gp_Pnt srcPnt;

    //! compute the mdl args which is the result of DHTrsfs[i]
    //! minus DHTrsfs[i-1]
    void ComputeMdlArgs();
};

#endif // RLAPI_DHSETTING_H
