#include "CustomDockWidgetTabBar.h"

CustomDockWidgetTabBar::CustomDockWidgetTabBar(Qt::DockWidgetArea area)
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

CustomDockWidgetTabBar::~CustomDockWidgetTabBar()
{
}

void CustomDockWidgetTabBar::insertSpacing()
{
	if(m_spacer != nullptr) {
        m_spacer->setFixedWidth(26);
	}
}

void CustomDockWidgetTabBar::removeSpacing()
{
	if(m_spacer != nullptr) {
		m_spacer->setFixedWidth(0);
	}
}

void CustomDockWidgetTabBar::addDockWidget(CustomDockWidget* dockWidget)
{
	if(dockWidget == nullptr) {
		return;
	}

	CustomDockWidgetTabButton* dockWidgetTabButton = new CustomDockWidgetTabButton(dockWidget->windowTitle(), dockWidget->getArea());

	connect(dockWidgetTabButton, &QPushButton::clicked, this, &CustomDockWidgetTabBar::dockWidgetButton_clicked);

	m_tabs[dockWidgetTabButton] = dockWidget;

	QAction* action = addWidget(dockWidgetTabButton);
	dockWidgetTabButton->setAction(action);

	if(m_tabs.size() == 1) {
	    show();
	}
}

bool CustomDockWidgetTabBar::removeDockWidget(CustomDockWidget* dockWidget)
{
	if(dockWidget == nullptr) {
		return false;
	}

	auto it = std::find_if(std::begin(m_tabs), std::end(m_tabs), [dockWidget](const std::pair<CustomDockWidgetTabButton*, CustomDockWidget*> v) {
		return v.second == dockWidget;
	} );

	if(it == m_tabs.end()) {
		return false;
	}

	CustomDockWidgetTabButton* dockWidgetTabButton = it->first;
		
	m_tabs.erase(it);

	removeAction(dockWidgetTabButton->getAction());

	if(m_tabs.empty()) {
		hide();
	}

	return true;
}

void CustomDockWidgetTabBar::dockWidgetButton_clicked()
{
	CustomDockWidgetTabButton* dockWidgetTabButton = dynamic_cast<CustomDockWidgetTabButton*>(sender());
	if(dockWidgetTabButton == nullptr) {
		return;
	}

	auto it = m_tabs.find(dockWidgetTabButton);
	if(it == m_tabs.end()) {
		return;
	}

	emit signal_dockWidgetButton_clicked(it->second);
}
