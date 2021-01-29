#pragma once

#include <QToolBar>
#include <QAction>

#include "CustomDockWidget.h"
#include "CustomDockTabBarPushButton.h"

class CustomDockTabBar : public QToolBar
{
	Q_OBJECT
public:
    explicit CustomDockTabBar(Qt::DockWidgetArea area);
    ~CustomDockTabBar();

public:
	void addDockWidget(CustomDockWidget* dockWidget);
	bool removeDockWidget(CustomDockWidget* dockWidget);

	void insertSpacing();
	void removeSpacing();

private:
	void dockWidgetButton_clicked();

signals:
	void signal_dockWidgetButton_clicked(CustomDockWidget* dockWidget);
	
private:
	Qt::DockWidgetArea m_area;
	QWidget* m_spacer;
    std::map<CustomDockTabBarPushButton*, CustomDockWidget*> m_tabs;
};
