#include "AIS_Coordinate.h"

IMPLEMENT_STANDARD_HANDLE(AIS_Coordinate,AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(AIS_Coordinate,AIS_InteractiveObject)

AIS_Coordinate::AIS_Coordinate()
{
    SetZLayer(Graphic3d_ZLayerId_Topmost);

    MyXColor = Quantity_NOC_RED4;
    MyYColor = Quantity_NOC_GREEN4;
    MyZColor = Quantity_NOC_BLUE4;

    MyRadiu = 3;
    Mylen = 120;
    MyNumber = 0;
    hasAttached = false;

    MyVX = gp_Vec(1,0,0);
    MyVY = gp_Vec(0,1,0);
    MyVZ = gp_Vec(0,0,1);
}

AIS_Coordinate::~AIS_Coordinate()
{
}

void AIS_Coordinate::Attach(const opencascade::handle<AIS_InteractiveObject> &targetObj)
{
    if (targetObj->IsKind (STANDARD_TYPE(AIS_Coordinate)))
    {
        return;
    }

    SetOwner(targetObj);
    Handle(AIS_Shape) targetShape = Handle(AIS_Shape)::DownCast(targetObj);
    MyPos = gp_Pnt(targetShape->LocalTransformation().TranslationPart());

    const Handle(AIS_InteractiveContext)& aContext = targetObj->GetContext();
    if (!aContext.IsNull())
    {
        if (!aContext->IsDisplayed (this))
        {
            aContext->Display (this, Standard_False);
        }
        else
        {
            aContext->Update (this, Standard_False);
            aContext->RecomputeSelectionOnly (this);
        }

        aContext->Load (this);
    }
    hasAttached = true;
}

void AIS_Coordinate::Detach()
{
    if (!hasAttached)
    {
        return;
    }

    const Handle(AIS_InteractiveContext)& aContext = this->GetContext();
    if (!aContext.IsNull())
    {
        aContext->Remove (this, Standard_False);
    }
    hasAttached = false;

    SetOwner(nullptr);
}

void AIS_Coordinate::SetAxisVec(const int &index, const gp_Vec &aVec)
{
    if(index>2||index<0)
        return;

    switch(index)
    {
    case 0:MyVX = aVec;break;
    case 1:MyVY = aVec;break;
    case 2:MyVZ = aVec;break;
    }
}

void AIS_Coordinate::Compute(const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,const Handle(Prs3d_Presentation)& aPresentation,
                        const Standard_Integer)
{
    aPresentation->Clear();
    aPresentation->SetInfiniteState (Standard_True);

    Handle(Prs3d_ShadingAspect) anAspect = new Prs3d_ShadingAspect();
    anAspect->Aspect()->SetInteriorStyle (Aspect_IS_SOLID);
    anAspect->SetMaterial (myDrawer->ShadingAspect()->Material());
    anAspect->SetTransparency (myDrawer->ShadingAspect()->Transparency());

    Handle(Graphic3d_Group) MyCoordGroup;
    // Display center
    mySphere.Init (MyRadiu * 2.0f, MyPos);
    MyCoordGroup = aPresentation->NewGroup ();
    MyCoordGroup->SetPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
    MyCoordGroup->AddPrimitiveArray (mySphere.Array());

    // Display Axes
    myAxes[0] = Axis (MyPos,MyVX, MyXColor,Mylen,MyRadiu);
    myAxes[1] = Axis (MyPos,MyVY, MyYColor,Mylen,MyRadiu);
    myAxes[2] = Axis (MyPos,MyVZ, MyZColor,Mylen,MyRadiu);

    for (Standard_Integer anIt = 0; anIt < 3; ++anIt)
    {
        MyCoordGroup = aPresentation->NewGroup();

        Handle(Prs3d_ShadingAspect) anAspectAx = new Prs3d_ShadingAspect (new Graphic3d_AspectFillArea3d(*anAspect->Aspect()));
        anAspectAx->SetColor (myAxes[anIt].Color());
        MyCoordGroup->SetGroupPrimitivesAspect (anAspectAx->Aspect());

        myAxes[anIt].Compute (thePrsMgr, aPresentation, anAspectAx);
        MyCoordGroup->AddPrimitiveArray(myAxes[anIt].Array());

        // Display text
        Handle(Prs3d_TextAspect) anAsp = myDrawer->TextAspect();
        anAsp->SetColor (Quantity_NOC_YELLOW);
        gp_Pnt MyEndPnt;
        TCollection_ExtendedString myText;
        if(anIt==0)
        {
            MyEndPnt = gp_Pnt(MyPos.Translated(gp_Vec(MyVX)*Mylen));
            myText.AssignCat("X");
        }
        else if(anIt==1)
        {
            MyEndPnt = gp_Pnt(MyPos.Translated(gp_Vec(MyVY)*Mylen));
            myText.AssignCat("Y");
        }
        else if(anIt==2)
        {
            MyEndPnt = gp_Pnt(MyPos.Translated(gp_Vec(MyVZ)*Mylen));
            myText.AssignCat("Z");
        }
        myText.AssignCat(TCollection_ExtendedString(MyNumber));
        Prs3d_Text::Draw (MyCoordGroup, anAsp, myText, MyEndPnt);
    }

    UpdateTransformation();
}

void AIS_Coordinate::ComputeSelection(const Handle(SelectMgr_Selection)& ,const Standard_Integer )
{
}

AIS_Coordinate::Axis::Axis(const gp_Pnt &theLoc,
                           const gp_Dir &theDir,
                           const Quantity_Color &theColor,
                           const Standard_ShortReal theLength,
                           const Standard_ShortReal theAxisRadius)
{
    myLocation = theLoc;
    myDirection = theDir;
    myColor = theColor;
    myLength = theLength;
    myAxisRadius = theAxisRadius;
}

void AIS_Coordinate::Axis::Compute(const opencascade::handle<PrsMgr_PresentationManager> &,
                                   const opencascade::handle<Prs3d_Presentation> &,
                                   const Handle(Prs3d_ShadingAspect)&)
{
    const Standard_Real anArrowLength   = 0.25 * myLength;

    myTriangleArray = Prs3d_Arrow::DrawShaded (gp_Ax1(myLocation,myDirection),
                                               myAxisRadius,
                                               myLength,
                                               myAxisRadius * 2,
                                               anArrowLength,
                                               20);
}

void AIS_Coordinate::Sphere::Init(const Standard_ShortReal theRadius,
                                  const gp_Pnt &thePosition,
                                  const Standard_Integer theSlicesNb,
                                  const Standard_Integer theStacksNb)
{
    myPosition = thePosition;
    myRadius = theRadius;

    Prs3d_ToolSphere aTool (theRadius, theSlicesNb, theStacksNb);
    gp_Trsf aTrsf;
    aTrsf.SetTranslation (gp_Vec(gp::Origin(), thePosition));
    aTool.FillArray (myArray, myTriangulation, aTrsf);
}
