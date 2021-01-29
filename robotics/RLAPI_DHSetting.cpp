#include "RLAPI_DHSetting.h"

//=======================================================================
//function : RLAPI_DHSetting
//purpose  : *
//=======================================================================
RLAPI_DHSetting::RLAPI_DHSetting()
    :theta({0,     0,      M_PI/2, 0,      0,      M_PI,    0}),
      d({   214,    116,    0,      0,      340,    0,      222}),
      alpha({0,     M_PI/2, 0,      -M_PI/2, M_PI/2, M_PI/2,    0}),
      a({    0,     -40,    345,    40,     0,      0,      61}),
      delta({0,0,0,0,0,0,0})
{
    srcPnt = gp_Pnt(0,0,0);

    for(int i=0;i<8;++i)
    {
        Handle(AIS_Coordinate) aCoord = new AIS_Coordinate;
        DHCoords.append(aCoord);
    }

    for(int j=0;j<8;++j)
    {
        DHCoords[j]->SetNumber(j);
    }
}

//=======================================================================
//function : ~RLAPI_DHSetting
//purpose  : *
//=======================================================================
RLAPI_DHSetting::~RLAPI_DHSetting()
{
    theta.clear();
    d.clear();
    alpha.clear();
    a.clear();
    delta.clear();
    DHTrsfs.clear();
    mdlArgs.clear();
    DHCoords.clear();
}

//=======================================================================
//function : Compute
//purpose  : *
//=======================================================================
void RLAPI_DHSetting::Compute()
{
    DHTrsfs.clear();

    gp_Trsf CumulativeTrsf;
    for(int i=0;i<7;++i)
    {
        gp_Trsf tmp;
        tmp.SetValues(cos(theta[i]), -sin(theta[i])*cos(alpha[i]), sin(theta[i])*sin(alpha[i]), a[i]*cos(theta[i]),
                      sin(theta[i]), cos(theta[i])*cos(alpha[i]), -cos(theta[i])*sin(alpha[i]), a[i]*sin(theta[i]),
                      0, sin(alpha[i]), cos(alpha[i]), d[i]);

        CumulativeTrsf.Multiply(tmp);

        DHTrsfs.append(CumulativeTrsf);
    }
}

//=======================================================================
//function : ComputeFK
//purpose  : *
//=======================================================================
void RLAPI_DHSetting::ComputeFK()
{
    Compute();

    if(!DHCoords[0]->LocalTransformation().TranslationPart().IsEqual(srcPnt.XYZ(),1e-6))
    {
        gp_Trsf pan;
        pan.SetValues(1,0,0,srcPnt.X(),
                      0,1,0,srcPnt.Y(),
                      0,0,1,srcPnt.Z());
        DHCoords[0]->SetLocalTransformation(pan);
    }

    for(int j=1;j<8;++j)
    {
        DHCoords[j]->SetPosition(gp_Pnt(0,0,0));
    }

    QList<gp_Ax1> mainAxis;
    for(int i=0;i<7;++i)
    {
        mainAxis.append(gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,1)));
    }

    gp_Trsf CumulativeTrsf;
    for(int i=0;i<7;++i)
    {
        mainAxis[i].Transform(DHTrsfs[i]);
        gp_Trsf rotation;
        rotation.SetRotation(mainAxis[i],delta[i]);

        CumulativeTrsf.Multiply(rotation);
        gp_Trsf workTrsf;
        workTrsf.Multiply(CumulativeTrsf);
        workTrsf.Multiply(DHTrsfs[i]);

        workTrsf.SetValues(workTrsf.Value(1,1),workTrsf.Value(1,2),workTrsf.Value(1,3),workTrsf.Value(1,4)+srcPnt.X(),
                           workTrsf.Value(2,1),workTrsf.Value(2,2),workTrsf.Value(2,3),workTrsf.Value(2,4)+srcPnt.Y(),
                           workTrsf.Value(3,1),workTrsf.Value(3,2),workTrsf.Value(3,3),workTrsf.Value(3,4)+srcPnt.Z());

        DHCoords[i+1]->SetLocalTransformation(workTrsf);
    }
}

//=======================================================================
//function : ApplyDHArgs
//purpose  : *
//=======================================================================
void RLAPI_DHSetting::ApplyDHArgs(const QString &mdlFileName)
{
    ComputeMdlArgs();
    RLAPI_Writer aWriter;
    aWriter.WriteArgsToMdlXMLFile(mdlFileName,mdlArgs);
}

//=======================================================================
//function : ComputeMdlArgs
//purpose  : *
//=======================================================================
void RLAPI_DHSetting::ComputeMdlArgs()
{
    mdlArgs.clear();

    mdlArgs.append(DHTrsfs[0].Value(1,4));
    mdlArgs.append(DHTrsfs[0].Value(2,4));
    mdlArgs.append(DHTrsfs[0].Value(3,4));

    for(int i=1;i<DHTrsfs.size();++i)
    {
        mdlArgs.append(DHTrsfs[i].Value(1,4)-DHTrsfs[i-1].Value(1,4));
        mdlArgs.append(DHTrsfs[i].Value(2,4)-DHTrsfs[i-1].Value(2,4));
        mdlArgs.append(DHTrsfs[i].Value(3,4)-DHTrsfs[i-1].Value(3,4));
    }
}
