#include "CustomDockTabBar.h"

CustomDockTabBar::CustomDockTabBar(Qt::DockWidgetArea area)
	: m_area(area)
{
	setObjectName("DockWidgetBar");

	setFloatable(false);
	setMovable(false);

	setContextMenuPolicy(Qt::PreventContextMenu);

	setOrientation(areaToOrientation(m_area));

    layout()->setSpacing(100);

	if(orientation() == Qt::Horizontal)
	{
		m_spacer = new QWidget();
		m_spacer->setFixedWidth(0);
		addWidget(m_spacer);
	}

	hide();
}

CustomDockTabBar::~CustomDockTabBar()
{
}

void CustomDockTabBar::insertSpacing()
{
	if(m_spacer != nullptr) {
        m_spacer->setFixedWidth(26);
	}
}

void CustomDockTabBar::removeSpacing()
{
	if(m_spacer != nullptr) {
		m_spacer->setFixedWidth(0);
	}
}

void CustomDockTabBar::addDockWidget(CustomDockWidget* dockWidget)
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

bool CustomDockTabBar::removeDockWidget(CustomDockWidget* dockWidget)
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
