#ifndef OCCWIDGET_H
#define OCCWIDGET_H

#include <QWidget>
#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QStyleFactory>

#include <AIS_Shape.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_View.hxx>
#include <WNT_Window.hxx>

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
    Standard_Integer midBtn_x;
    Standard_Integer midBtn_y;

    QPoint startPnt;
    QPoint endPnt;

signals:
    void pickPixel(int x ,int y);
};

#endif // OCCWIDGET_H
