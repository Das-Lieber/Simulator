#include "mainwindow.h"

CustomDockWidget::CustomDockWidget()
	: QDockWidget(nullptr)
	, m_area(Qt::NoDockWidgetArea)
	, m_state(DockWidgetState::Unknown)
    , contents(nullptr)
{
	setAutoFillBackground(true);

    m_titleWidget = new CustomDockWidgetBar();

	setTitleBarWidget(m_titleWidget);


	m_layout = new QVBoxLayout;
	m_layout->setContentsMargins(0, 2, 0, 0);

	QWidget* widget = new QWidget();
	widget->setLayout(m_layout);
	QDockWidget::setWidget(widget);

    connect(m_titleWidget, &CustomDockWidgetBar::menuButton_pressed, this, &CustomDockWidget::openTitleMenu);
    connect(m_titleWidget, &CustomDockWidgetBar::autoHideButton_pressed, this, &CustomDockWidget::autoHideStateToggle);
    connect(m_titleWidget, &CustomDockWidgetBar::closeButton_pressed, this, &CustomDockWidget::closeDockWidget);

    connect(this, &QDockWidget::dockLocationChanged, this, &CustomDockWidget::updateDockLocation);
    connect(this, &QDockWidget::topLevelChanged, this, &CustomDockWidget::updateTopLevelState);
}

CustomDockWidget::~CustomDockWidget()
{
}
	
void CustomDockWidget::setWindowTitle(const QString& text)
{
	QString title = text.isEmpty() ? "Noname" : text;

	m_titleWidget->setText(title);
	QDockWidget::setWindowTitle(title);
}

void CustomDockWidget::closeDockWidget()
{
    if(contents!=nullptr)
        contents->deleteLater();
	if(isMinimized()) {
		emit signal_pinned(this);
	}

	setState(DockWidgetState::Closed);

	hide();
}

void CustomDockWidget::openTitleMenu()
{
	QMenu menu;
	menu.addAction("Float", this, SLOT(slot_menuAction()));
	menu.addAction("Dock", this, SLOT(slot_menuAction()));
	menu.addAction("Auto Hide", this, SLOT(slot_menuAction()));
	menu.addAction("Hide", this, SLOT(slot_menuAction()));

	menu.exec(m_titleWidget->menuPos());
}

void CustomDockWidget::autoHideStateToggle()
{
	if(isMinimized()) 
	{
		setState(DockWidgetState::Docked);
		emit signal_pinned(this);
	}
	else 
	{
		setState(DockWidgetState::Hidden);
		emit signal_unpinned(this);
	}
}

void CustomDockWidget::updateDockLocation(Qt::DockWidgetArea area)
{
	m_area = area;

	if(m_area != Qt::NoDockWidgetArea) {
		updateTopLevelState(false);
	}
}

void CustomDockWidget::updateTopLevelState(bool topLevel)
{
	m_titleWidget->setAutoHideEnadled(false);

	if(topLevel)
	{
		setState(DockWidgetState::Floating);

        std::for_each(m_tabifieds.begin(), m_tabifieds.end(), [&](CustomDockWidget* dockWidget) {
			dockWidget->removeFromTabifiedDocks(this);
 		} );

		clearTabifiedDocks();

		emit signal_undocked(this);
	}
	else
	{
		setState(DockWidgetState::Docked);

        QList<QDockWidget*> tabifiedDockWidgetList = static_cast<MainWindow*>(parentWidget())->tabifiedDockWidgets(this);
		tabifiedDockWidgetList.push_back(this);

		std::for_each(std::begin(tabifiedDockWidgetList), std::end(tabifiedDockWidgetList), [&](QDockWidget* qDockWidget) {
            qobject_cast<CustomDockWidget*>(qDockWidget)->setTabifiedDocks(tabifiedDockWidgetList);
		} );

		emit signal_docked(this);
	}
}

void CustomDockWidget::setState(DockWidgetState state)
{
	m_state = state;

	switch(state)
	{
		case DockWidgetState::Docked:
			m_titleWidget->setFloating(true);
			break;
		case DockWidgetState::Floating:
			m_titleWidget->setFloating(false);
			break;
		case DockWidgetState::Hidden:
			m_titleWidget->setAutoHideEnadled(true);
			break;
		case DockWidgetState::Closed:
			break;
		default:
			break;
	}
}

bool CustomDockWidget::event(QEvent* event)
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

void CustomDockWidget::setWidget(QWidget* widget)
{
	m_layout->addWidget(widget);
    setWindowTitle(widget->windowTitle());
    contents = widget;
}

void CustomDockWidget::addWidget(QWidget *widget)
{
    m_layout->addWidget(widget);
}

void CustomDockWidget::removeWidget()
{
    if(contents!=nullptr)
        m_layout->removeWidget(contents);
    contents->deleteLater();
}

void CustomDockWidget::setTabifiedDocks(const QList<QDockWidget*>& dockWidgetList)
{
	m_tabifieds.clear();

	std::transform(std::begin(dockWidgetList), std::end(dockWidgetList), std::back_inserter(m_tabifieds), [&](QDockWidget* qDockWidget) {
        return static_cast<CustomDockWidget*>(qDockWidget);
	} );
}

void CustomDockWidget::removeFromTabifiedDocks(CustomDockWidget* dockWidget)
{
	m_tabifieds.erase(std::remove(std::begin(m_tabifieds), std::end(m_tabifieds), dockWidget), std::end(m_tabifieds));
}
