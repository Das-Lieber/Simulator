#ifndef AIS_COORDINATE_H
#define AIS_COORDINATE_H

#include <Standard_DefineHandle.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_Root.hxx>
#include <AIS_Shape.hxx>
#include <Poly_Triangulation.hxx>
#include <Prs3d_ToolSphere.hxx>

class AIS_Coordinate:public AIS_InteractiveObject
{
public:
    AIS_Coordinate();
    ~AIS_Coordinate() override;

    void Attach(const Handle(AIS_InteractiveObject) &targetObj);
    void Detach();
    Standard_Boolean IsAttached() {
        return hasAttached;
    }
    void SetAxisVec(const int& index, const gp_Vec& aVec);
    void SetPosition(const gp_Pnt& aPnt) {
        MyPos = aPnt;
    }
    void SetNumber(const Standard_Integer aNumber) {
        MyNumber = aNumber;
    }

private:    

    Standard_EXPORT void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                                 const Handle(Prs3d_Presentation)& aPresentation,
                                 const Standard_Integer aMode = 0) override;
    Standard_EXPORT void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                          const Standard_Integer aMode) override;

private:
    bool hasAttached;
    gp_Pnt MyPos;
    Standard_ShortReal Mylen;
    Standard_ShortReal MyRadiu;
    Standard_Integer MyNumber;

    Quantity_Color MyXColor;
    Quantity_Color MyYColor;
    Quantity_Color MyZColor;

    gp_Dir MyVX;
    gp_Dir MyVY;
    gp_Dir MyVZ;

    class Axis
    {
    public:
        Axis(){}
        Axis (const gp_Pnt& theLoc,
              const gp_Dir& theDir,
              const Quantity_Color& theColor,
              const Standard_ShortReal theLength,
              const Standard_ShortReal theAxisRadius);

        void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                      const Handle(Prs3d_Presentation)& thePrs,
                      const Handle(Prs3d_ShadingAspect)& theAspect);
        const Handle(Graphic3d_ArrayOfTriangles)& Array() const { return myTriangleArray; }
        Quantity_Color Color() const { return myColor; }

    protected:

        gp_Pnt myLocation;
        gp_Dir myDirection;
        Quantity_Color myColor;
        Standard_ShortReal myLength;
        Standard_ShortReal myAxisRadius;

        Handle(Graphic3d_ArrayOfTriangles) myTriangleArray;

    };

    class Sphere
    {
    public:
        Sphere(){}
        ~Sphere()
        {
            myTriangulation.Nullify();
            myArray.Nullify();
        }
        const Handle(Graphic3d_ArrayOfTriangles)& Array() const { return myArray; }

        void Init (const Standard_ShortReal theRadius,
                   const gp_Pnt& thePosition,
                   const Standard_Integer theSlicesNb = 20,
                   const Standard_Integer theStacksNb = 20);

    protected:
        gp_Pnt myPosition;
        Standard_ShortReal myRadius;
        Handle(Poly_Triangulation) myTriangulation;
        Handle(Graphic3d_ArrayOfTriangles) myArray;
    };

    Axis myAxes[3];
    Sphere mySphere;

public:
    DEFINE_STANDARD_RTTIEXT(AIS_Coordinate,AIS_InteractiveObject)
};

DEFINE_STANDARD_HANDLE(AIS_Coordinate,AIS_InteractiveObject)
#endif
