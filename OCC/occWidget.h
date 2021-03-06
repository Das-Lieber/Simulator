#ifndef OCCWIDGET_H
#define OCCWIDGET_H

#include <QWidget>
#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QStyleFactory>
#include <QRubberBand>

#include <AIS_Shape.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_View.hxx>
#include <WNT_Window.hxx>
#include <AIS_Manipulator.hxx>
#include <AIS_ViewCube.hxx>

class OCCWidget:public QWidget
{
    Q_OBJECT

public:
    OCCWidget(QWidget *parent=nullptr);//set the parent arg to avoid error

    Handle(V3d_View) getView()
    {
        return m_view;
    }

    Handle(AIS_InteractiveContext) getContext()
    {
        return m_context;
    }

    Handle(AIS_Manipulator) getManipulator()
    {
        return m_manipulator;
    }

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    QPaintEngine *paintEngine() const;

private:
    Handle(AIS_InteractiveContext) m_context;
    Handle(V3d_Viewer) m_viewer;
    Handle(V3d_View) m_view;
    Handle(Graphic3d_GraphicDriver) m_graphicDriver;
    Handle(AIS_Manipulator) m_manipulator;
    Standard_Integer midBtn_x;
    Standard_Integer midBtn_y;

    QPoint startPnt;
    QPoint endPnt;
    QRubberBand *m_rubberBand;

signals:
    void pickPixel(int x ,int y);
    void selectShapeChanged(const TopoDS_Shape selected);
};

#endif // OCCWIDGET_H
