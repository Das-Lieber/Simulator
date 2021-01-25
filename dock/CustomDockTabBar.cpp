#include "CustomDockTabBar.h"

CustomDockTabBar::CustomDockTabBar(Qt::DockWidgetArea area) : m_area(area)
{
    setAttribute(Qt::WA_StyledBackground);
    setFloatable(false);
    setMovable(false);
    setContextMenuPolicy(Qt::PreventContextMenu);
    setOrientation(areaToOrientation(m_area));
    layout()->setSpacing(15);
    hide();
}

void CustomDockTabBar::addDockWidget(CustomDockWidget *dockWidget)
{
    if(dockWidget == nullptr) {
        return;
    }

    CustomDockTabBarPushButton* dockWidgetTabButton = new CustomDockTabBarPushButton(dockWidget->windowTitle(), dockWidget->getArea());

    connect(dockWidgetTabButton, &QPushButton::clicked, this, &CustomDockTabBar::dockWidgetButton_clicked);

    m_tabs[dockWidgetTabButton] = dockWidget;

    QAction* action = addWidget(dockWidgetTabButton);
    dockWidgetTabButton->setAction(action);

    if(m_tabs.size() == 1) {
        show();
    }
}

bool CustomDockTabBar::removeDockWidget(CustomDockWidget *dockWidget)
{
    if(dockWidget == nullptr) {
        return false;
    }

    auto it = std::find_if(std::begin(m_tabs), std::end(m_tabs), [dockWidget](const std::pair<CustomDockTabBarPushButton*, CustomDockWidget*> v) {
        return v.second == dockWidget;
    } );

    if(it == m_tabs.end()) {
        return false;
    }

    CustomDockTabBarPushButton* dockWidgetTabButton = it->first;

    m_tabs.erase(it);

    removeAction(dockWidgetTabButton->getAction());

    if(m_tabs.empty()) {
        hide();
    }

    return true;
}

Qt::Orientation CustomDockTabBar::areaToOrientation(Qt::DockWidgetArea area)
{
    Q_ASSERT((area == Qt::LeftDockWidgetArea) || (area ==Qt::RightDockWidgetArea ) ||
           (area == Qt::TopDockWidgetArea) || (area == Qt::BottomDockWidgetArea));

    switch(area)
    {
        case Qt::LeftDockWidgetArea:
        case Qt::RightDockWidgetArea:
            return Qt::Vertical;
        case Qt::TopDockWidgetArea:
        case Qt::BottomDockWidgetArea:
            return Qt::Horizontal;
        default:
            return Qt::Orientation(0);
    }
}

void CustomDockTabBar::dockWidgetButton_clicked()
{
    CustomDockTabBarPushButton* dockWidgetTabButton = dynamic_cast<CustomDockTabBarPushButton*>(sender());
    if(dockWidgetTabButton == nullptr) {
        return;
    }

    auto it = m_tabs.find(dockWidgetTabButton);
    if(it == m_tabs.end()) {
        return;
    }

    emit signal_dockWidgetButton_clicked(it->second);
}
