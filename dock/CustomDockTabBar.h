#ifndef CUSTOMDOCKTABBAR_H
#define CUSTOMDOCKTABBAR_H

#include <QToolBar>
#include <QAction>

#include "CustomDockWidget.h"
#include "CustomDockTabBarPushButton.h"

class CustomDockTabBar : public QToolBar
{
    Q_OBJECT
public:
    explicit CustomDockTabBar(Qt::DockWidgetArea area);

    void addDockWidget(CustomDockWidget *dockWidget);
    bool removeDockWidget(CustomDockWidget *dockWidget);

private:
    Qt::DockWidgetArea m_area;
    std::map<CustomDockTabBarPushButton*, CustomDockWidget*> m_tabs;

    Qt::Orientation areaToOrientation(Qt::DockWidgetArea area);
    void dockWidgetButton_clicked();

signals:
    void signal_dockWidgetButton_clicked(CustomDockWidget* dockWidget);
};

#endif // CUSTOMDOCKTABBAR_H
