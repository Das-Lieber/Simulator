#include "RLAPI_DHSetting.h"

//=======================================================================
//function : RLAPI_DHSetting
//purpose  : *
//=======================================================================
RLAPI_DHSetting::RLAPI_DHSetting()
    :theta({0,     0,      M_PI/2, 0,      0,      M_PI,    0}),
      d({   214,    116,    0,      0,      340,    0,      222}),
      alpha({0,     M_PI/2, 0,      -M_PI/2, M_PI/2, M_PI/2,    0}),
      a({    0,     -40,    345,    40,     0,      0,      61})
{
    srcPnt = gp_Pnt(0,0,0);

    for(int i=0;i<8;++i)
    {
        Handle(AIS_Coordinate) aCoord = new AIS_Coordinate;
        DHCoords.append(aCoord);
    }
    DHCoords[0]->SetPosition(srcPnt);
    for(int j=1;j<8;++j)
    {
        DHCoords[j]->SetNumber(j);
    }
}

//=======================================================================
//function : Compute
//purpose  : *
//=======================================================================
void RLAPI_DHSetting::Compute()
{
    DHTrsfs.clear();
    for(int j=1;j<8;++j)
    {
        DHCoords[j]->SetPosition(gp_Pnt(0,0,0));
    }

    for(int i=0;i<7;++i)
    {
        gp_Trsf tmp;
        tmp.SetValues(cos(theta[i]), -sin(theta[i])*cos(alpha[i]), sin(theta[i])*sin(alpha[i]), a[i]*cos(theta[i]),
                      sin(theta[i]), cos(theta[i])*cos(alpha[i]), -cos(theta[i])*sin(alpha[i]), a[i]*sin(theta[i]),
                      0, sin(alpha[i]), cos(alpha[i]), d[i]);

        //            qDebug()<<tmp.Value(1,1)<<tmp.Value(1,2)<<tmp.Value(1,3)<<tmp.Value(1,4)<<endl
        //                   <<tmp.Value(2,1)<<tmp.Value(2,2)<<tmp.Value(2,3)<<tmp.Value(2,4)<<endl
        //                  <<tmp.Value(3,1)<<tmp.Value(3,2)<<tmp.Value(3,3)<<tmp.Value(3,4)<<endl;

        if(i==0)
            DHTrsfs.append(tmp);
        else
            DHTrsfs.append(DHTrsfs.last().Multiplied(tmp));

        //            qDebug()<<DHTrsfs.last().Value(1,1)<<DHTrsfs.last().Value(1,2)<<DHTrsfs.last().Value(1,3)<<DHTrsfs.last().Value(1,4)<<endl
        //                   <<DHTrsfs.last().Value(2,1)<<DHTrsfs.last().Value(2,2)<<DHTrsfs.last().Value(2,3)<<DHTrsfs.last().Value(2,4)<<endl
        //                  <<DHTrsfs.last().Value(3,1)<<DHTrsfs.last().Value(3,2)<<DHTrsfs.last().Value(3,3)<<DHTrsfs.last().Value(3,4)<<endl;

        DHCoords[i+1]->SetLocalTransformation(DHTrsfs.last());
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
