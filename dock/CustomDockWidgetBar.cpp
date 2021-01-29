#include "CustomDockWidgetBar.h"
#include "CustomDockWidget.h"

static const QString s_autoHideDisabledStyle =
        R"(QPushButton
        {
        border: 0px;
        width: 15px; height: 15px;
        background: transparent;
        image: url(:/dock/icons/pin_dockwidget_normal.png);
        }
        QPushButton:hover {
        image: url(:/dock/icons/pin_dockwidget_hover.png);
        }
        QPushButton:pressed:hover {
        image: url(:/dock/icons/pin_dockwidget_normal.png);
        })";

static const QString s_autoHideEnabledStyle =
        R"(QPushButton
        {
        border: 0px;
        width: 15px; height: 15px;
        background: transparent;
        image: url(:/dock/icons/unpin_dockwidget_normal.png);
        }
        QPushButton:hover {
        image: url(:/dock/icons/unpin_dockwidget_hover.png);
        }
        QPushButton:pressed:hover {
        image: url(:/dock/icons/unpin_dockwidget_pressed.png);
        })";

static const QString s_closeButtonStyle =
        R"(QPushButton
        {
        border: 0px;
        width: 15px; height: 15px;
        background: transparent;
        image: url(:/dock/icons/close_dockwidget_normal.png);
        }
        QPushButton:hover {
        image: url(:/dock/icons/close_dockwidget_hover.png);
        }
        QPushButton:pressed:hover {
        image: url(:/dock/icons/close_dockwidget_pressed.png);
        })";

static const QString s_menuButtonStyle =
        R"(QPushButton
        {
        border: 0px;
        width: 15px; height: 15px;
        background: transparent;
        image: url(:/dock/icons/menu_dockwidget_normal.png);
        }
        QPushButton:hover {
        image: url(:/dock/icons/menu_dockwidget_hover.png);
        }
        QPushButton:pressed:hover {
        image: url(:/dock/icons/menu_dockwidget_pressed.png);
        })";

CustomDockWidgetBar::CustomDockWidgetBar()
	: QFrame(nullptr)
	, m_LMPressed(false)
	, m_textLabel(nullptr)
    , m_autoHideEnabled(false)
{
	setObjectName("DockWidgetTitle");

	QHBoxLayout* layout = new QHBoxLayout();
	setLayout(layout);

	layout->setContentsMargins(3, 2, 3, 2);
	layout->setSpacing(1);

	m_textLabel = new QLabel();

	layout->addWidget(m_textLabel);

	layout->addStretch(1);

	m_menuButton = new QPushButton();
	m_menuButton->setStyleSheet(s_menuButtonStyle);
	m_menuButton->setToolTip(tr("Menu"));
	layout->addWidget(m_menuButton);

	m_autoHideButton = new QPushButton();
	m_autoHideButton->setStyleSheet(s_autoHideDisabledStyle);
	m_autoHideButton->setToolTip(tr("Auto Hide"));
	m_autoHideEnabled = true;
	layout->addWidget(m_autoHideButton);

	m_closeButton = new QPushButton();
	m_closeButton->setStyleSheet(s_closeButtonStyle);
	m_closeButton->setToolTip(tr("Close"));
	layout->addWidget(m_closeButton);

    connect(m_menuButton, &QPushButton::clicked, this, &CustomDockWidgetBar::menuButton_pressed);
    connect(m_autoHideButton, &QPushButton::clicked, this, &CustomDockWidgetBar::autoHideButton_pressed);
    connect(m_closeButton, &QPushButton::clicked, this, &CustomDockWidgetBar::closeButton_pressed);
}

CustomDockWidgetBar::~CustomDockWidgetBar()
{
}

void CustomDockWidgetBar::setFloating(bool state)
{
    m_autoHideButton->setVisible(state);
}

void CustomDockWidgetBar::setAutoHideEnadled(bool enabled)
{
	m_autoHideEnabled = enabled;

	if(enabled) {
		m_autoHideButton->setStyleSheet(s_autoHideEnabledStyle);
	}
	else {	
		m_autoHideButton->setStyleSheet(s_autoHideDisabledStyle);
	}
}

QPoint CustomDockWidgetBar::menuPos() const
{
	QPoint p = m_menuButton->pos();
	p.ry() += m_menuButton->height();

	return QPoint(mapToGlobal(p));
}

void CustomDockWidgetBar::mousePressEvent(QMouseEvent* event)
{
    if((event->button() == Qt::LeftButton) && m_autoHideEnabled) {
        m_LMPressed = true;
    }

    QFrame::mousePressEvent(event);
}

void CustomDockWidgetBar::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton) {
        m_LMPressed = false;
    }

    QFrame::mouseReleaseEvent(event);
}

void CustomDockWidgetBar::mouseMoveEvent(QMouseEvent* event)
{
    if(m_LMPressed)
    {
        CustomDockWidget* dockWidget = static_cast<CustomDockWidget*>(parentWidget());
        if(dockWidget != nullptr)
        {
            m_LMPressed = false;

            dockWidget->setFloating(true);

            event = new QMouseEvent(QEvent::MouseButtonPress,
                                    event->pos(),
                                    mapToGlobal(event->pos()),
                                    Qt::LeftButton,
                                    Qt::LeftButton,
                                    Qt::NoModifier);

            QCoreApplication::postEvent(this, event);
        }
    }

    QFrame::mouseMoveEvent(event);
}
