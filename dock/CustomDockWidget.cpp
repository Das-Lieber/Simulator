#include "CustomDockWidget.h"

CustomDockWidget::CustomDockWidget(QWidget *parent):
    QDockWidget(parent),
    dockWidgetState(DockWidgetState::Unknown),
    m_area(Qt::NoDockWidgetArea)
{
    titleWidget = new CustomDockWidgetBar;
    this->setTitleBarWidget(titleWidget);
    setAttribute(Qt::WA_StyledBackground);//use qss to set this object

    globalLayout = new QVBoxLayout;
    globalLayout->setContentsMargins(0, 2, 0, 0);

    QWidget* widget = new QWidget();
    widget->setLayout(globalLayout);
    QDockWidget::setWidget(widget);

    connect(titleWidget, &CustomDockWidgetBar::menuButton_pressed, this,[=]() {
        QMenu menu;
        menu.addAction("Float", this, SLOT(slot_menuAction()));
        menu.addAction("Dock", this, SLOT(slot_menuAction()));
        menu.addAction("Auto Hide", this, SLOT(slot_menuAction()));
        menu.addAction("Hide", this, SLOT(slot_menuAction()));

        menu.exec(titleWidget->menuPos());
    });

    connect(titleWidget, &CustomDockWidgetBar::autoHideButton_pressed, this, [=](){
        if(isMinimized())
        {
            setDockWidgetState(DockWidgetState::Docked);
            emit signal_pinned(this);
        }
        else
        {
            setDockWidgetState(DockWidgetState::Hidden);
            emit signal_unpinned(this);
        }
    });

    connect(titleWidget, &CustomDockWidgetBar::closeButton_pressed, this, [=](){
        if(isMinimized()) {
            emit signal_pinned(this);
        }

        setDockWidgetState(DockWidgetState::Closed);

        hide();
    });

    connect(this, &QDockWidget::dockLocationChanged, this, &CustomDockWidget::updateDockLocation);
    connect(this, &QDockWidget::topLevelChanged, this, &CustomDockWidget::updateTopLevelState);
}

void CustomDockWidget::setWidget(QWidget *widget)
{
    globalLayout->addWidget(widget);
    setWindowTitle(widget->windowTitle());
}

void CustomDockWidget::setDockWidgetState(DockWidgetState state)
{
    dockWidgetState = state;
    switch(state)
    {
        case DockWidgetState::Docked:
            titleWidget->setFloating(true);
            break;
        case DockWidgetState::Floating:
            titleWidget->setFloating(false);
            break;
        case DockWidgetState::Hidden:
            titleWidget->setAutoHideEnadled(true);
            break;
        case DockWidgetState::Closed:
            break;
        default:
            break;
    }
}

void CustomDockWidget::setTabifiedDocks(const QList<QDockWidget *> &dockWidgetList)
{
    m_tabifieds.clear();

    std::transform(std::begin(dockWidgetList), std::end(dockWidgetList), std::back_inserter(m_tabifieds), [&](QDockWidget* qDockWidget) {
        return static_cast<CustomDockWidget*>(qDockWidget);
    } );
}

bool CustomDockWidget::event(QEvent *event)
{
    if(event->type() == QEvent::Enter)
    {
    }
    else if(event->type() == QEvent::Leave)
    {
    }
    else if(event->type() == QEvent::FocusOut)
    {
    }

    // Make sure the rest of events are handled
    return QDockWidget::event(event);
}

void CustomDockWidget::updateDockLocation(Qt::DockWidgetArea area)
{
    m_area = area;
    if(area != Qt::NoDockWidgetArea) {
        updateTopLevelState(false);
    }
}

void CustomDockWidget::updateTopLevelState(bool topLevel)
{
    titleWidget->setAutoHideEnadled(false);

    if(topLevel)
    {
        setDockWidgetState(DockWidgetState::Floating);

        std::for_each(m_tabifieds.begin(), m_tabifieds.end(), [&](CustomDockWidget* dockWidget) {
            dockWidget->removeFromTabifiedDocks(this);
        } );

        clearTabifiedDocks();

        emit signal_undocked(this);
    }
    else
    {
        setDockWidgetState(DockWidgetState::Docked);

        QList<QDockWidget*> tabifiedDockWidgetList = static_cast<QMainWindow*>(parentWidget())->tabifiedDockWidgets(this);
        tabifiedDockWidgetList.push_back(this);

        std::for_each(std::begin(tabifiedDockWidgetList), std::end(tabifiedDockWidgetList), [&](QDockWidget* qDockWidget) {
            qobject_cast<CustomDockWidget*>(qDockWidget)->setTabifiedDocks(tabifiedDockWidgetList);
        } );

        emit signal_docked(this);
    }
}

void CustomDockWidget::removeFromTabifiedDocks(CustomDockWidget *dockWidget)
{
    m_tabifieds.erase(std::remove(std::begin(m_tabifieds), std::end(m_tabifieds), dockWidget), std::end(m_tabifieds));
}
