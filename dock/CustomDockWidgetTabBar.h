#pragma once

#include <QToolBar>
#include <QAction>

#include "CustomDockWidget.h"
#include "CustomDockWidgetTabButton.h"

class CustomDockWidgetTabBar : public QToolBar
{
	Q_OBJECT
public:
    explicit CustomDockWidgetTabBar(Qt::DockWidgetArea area);
    ~CustomDockWidgetTabBar();

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
	std::map<CustomDockWidgetTabButton*, CustomDockWidget*> m_tabs;
};
