#include "occWidget.h"

OCCWidget::OCCWidget(QWidget *parent) :QWidget(parent)
{
    if (m_context.IsNull())
    {
        Handle(Aspect_DisplayConnection) m_display_donnection = new Aspect_DisplayConnection();
        if (m_graphicDriver.IsNull())
        {
            m_graphicDriver = new OpenGl_GraphicDriver(m_display_donnection);
        }
        WId window_handle =  winId();
        Handle(WNT_Window) wind = new WNT_Window((Aspect_Handle) window_handle);
        m_viewer = new V3d_Viewer(m_graphicDriver);
        m_view = m_viewer->CreateView();
        m_view->SetWindow(wind);

        if (!wind->IsMapped())
        {
            wind->Map();
        }
        m_context = new AIS_InteractiveContext(m_viewer);
        m_viewer->SetDefaultLights();
        m_viewer->SetLightOn();
        m_view->ChangeRenderingParams().Method = Graphic3d_RM_RASTERIZATION;
        m_view->ChangeRenderingParams().IsAntialiasingEnabled = Standard_False;
        m_view->SetBgGradientColors(Quantity_NOC_GRAY80,Quantity_NOC_WHITESMOKE,Aspect_GFM_VER);
        m_view->MustBeResized();
        m_context->SetDisplayMode(AIS_Shaded, Standard_True);
    }

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setBackgroundRole( QPalette::NoRole );
    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking(true);

    m_context->HighlightStyle()->SetColor(Quantity_NOC_CYAN1);
    m_context->SelectionStyle()->SetColor(Quantity_NOC_CYAN1);
    m_context->HighlightStyle(Prs3d_TypeOfHighlight_LocalDynamic)->SetColor(Quantity_NOC_CYAN1);
    m_context->HighlightStyle(Prs3d_TypeOfHighlight_LocalSelected)->SetColor(Quantity_NOC_CYAN1);

    m_context->HighlightStyle()->SetDisplayMode(1);
    m_context->SelectionStyle()->SetDisplayMode(1);
    m_context->SelectionStyle()->SetTransparency(0.2f);
    m_context->HighlightStyle()->SetTransparency(0.2f);

    m_context->MainSelector()->SetPickClosest(Standard_False);

    m_rubberBand = new QRubberBand(QRubberBand::Rectangle,this);
    m_rubberBand->setStyle(QStyleFactory::create("windows"));

    m_manipulator = new AIS_Manipulator();
    m_manipulator->SetPart (0, AIS_MM_Scaling, Standard_True);
    m_manipulator->SetPart (1, AIS_MM_Rotation, Standard_True);
    m_manipulator->SetPart (2, AIS_MM_Translation, Standard_True);
    m_manipulator->EnableMode (AIS_MM_Translation);
    m_manipulator->EnableMode (AIS_MM_Rotation);
    m_manipulator->EnableMode (AIS_MM_Scaling);

    Handle(AIS_ViewCube) aCube = new AIS_ViewCube();
    aCube->SetAutoStartAnimation(Standard_True);
    aCube->SetSize(60);
    aCube->SetFontHeight(13);
    m_context->Display(aCube,Standard_False);
}

void OCCWidget::paintEvent(QPaintEvent *)
{
    m_view->Redraw();
}

void OCCWidget::resizeEvent(QResizeEvent *)
{
    if( !m_view.IsNull() )
    {
        m_view->MustBeResized();
    }
}

void OCCWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::RightButton)
    {
        m_view->StartRotation(event->x(),event->y());
    }
    else if(event->button()==Qt::LeftButton)
    {
        m_context->MoveTo(event->pos().x(),event->pos().y(),m_view,Standard_True);
        startPnt = event->pos();

        if(!m_manipulator->IsAttached())
            m_rubberBand->setGeometry(QRect(startPnt,QSize(1,1)));
        else
            m_manipulator->StartTransform(event->pos().x(),event->pos().y(),m_view);
    }
    else if(event->button() == Qt::MidButton)
    {
        midBtn_x=event->x();
        midBtn_y=event->y();
    }
}

void OCCWidget::mouseReleaseEvent(QMouseEvent *event)
{
    unsetCursor();
    m_context->MoveTo(event->pos().x(),event->pos().y(),m_view,Standard_True);    

    if(event->button()==Qt::LeftButton)
    {
        if(!m_rubberBand->isHidden()&&!m_manipulator->IsAttached())
            m_rubberBand->hide();

        if(m_manipulator->IsAttached())
            m_manipulator->StopTransform();

        if(event->pos()==startPnt)
        {
            AIS_StatusOfPick t_pick_status = AIS_SOP_NothingSelected;
            if(qApp->keyboardModifiers()==Qt::ControlModifier)
            {
                t_pick_status = m_context->ShiftSelect(true);
            }
            else
            {
                t_pick_status = m_context->Select(true);
                m_context-> InitSelected();
            }

            emit pickPixel(event->x(),event->y());
        }
        else
        {
            m_context->Select(startPnt.x(),startPnt.y(),event->x(),event->y(),m_view,Standard_True);
            m_context-> InitSelected();
        }
    }
}

void OCCWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()&Qt::RightButton)
    {
        setCursor(Qt::ClosedHandCursor);
        m_view->Rotation(event->x(),event->y());
    }
    else if(event->buttons()&Qt::MidButton)
    {
        setCursor(Qt::SizeAllCursor);
        m_view->Pan(event->pos().x()-midBtn_x,midBtn_y-event->pos().y());
        midBtn_x=event->x();
        midBtn_y=event->y();
    }
    else if(event->buttons()&Qt::LeftButton)
    {
        if(!m_manipulator->IsAttached())
        {
            m_rubberBand->show();
            m_rubberBand->setGeometry(QRect(startPnt,event->pos()).normalized());
        }
        else
        {
            m_manipulator->Transform(event->pos().x(),event->pos().y(),m_view);
            m_view->Redraw();
        }
    }
    else
    {
        m_context->MoveTo(event->pos().x(),event->pos().y(),m_view,Standard_True);
    }
}

void OCCWidget::wheelEvent(QWheelEvent *event)
{
    m_view->StartZoomAtPoint(event->pos().x(),event->pos().y());
    m_view->ZoomAtPoint(0, 0, event->angleDelta().y(), 0);
}

QPaintEngine *OCCWidget::paintEngine() const
{
    return nullptr;
}
