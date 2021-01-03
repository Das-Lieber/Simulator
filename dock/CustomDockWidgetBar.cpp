#include "CustomDockWidgetBar.h"

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

CustomDockWidgetBar::CustomDockWidgetBar(QWidget *parent)
    : QWidget(parent),
      leftMousePressed(false),
      titleLabel(nullptr),
      autoHideEnabled(false)
{
    setAttribute(Qt::WA_StyledBackground);
    QHBoxLayout* layout = new QHBoxLayout();
    setLayout(layout);

    layout->setContentsMargins(3, 2, 3, 2);
    layout->setSpacing(1);

    titleLabel = new QLabel();

    layout->addWidget(titleLabel);

    layout->addStretch(1);

    menuButton = new QPushButton();
    menuButton->setToolTip(tr("Menu"));
    menuButton->setStyleSheet(s_menuButtonStyle);
    layout->addWidget(menuButton);

    autoHideButton = new QPushButton();
    autoHideButton->setToolTip(tr("Auto Hide"));
    autoHideButton->setStyleSheet(s_autoHideDisabledStyle);
    autoHideEnabled = true;
    layout->addWidget(autoHideButton);

    closeButton = new QPushButton();
    closeButton->setToolTip(tr("Close"));
    closeButton->setStyleSheet(s_closeButtonStyle);
    layout->addWidget(closeButton);

    connect(menuButton, &QPushButton::clicked, this, &CustomDockWidgetBar::menuButton_pressed);
    connect(autoHideButton, &QPushButton::clicked, this, &CustomDockWidgetBar::autoHideButton_pressed);
    connect(closeButton, &QPushButton::clicked, this, &CustomDockWidgetBar::closeButton_pressed);
}

void CustomDockWidgetBar::setFloating(bool state)
{
    autoHideButton->setVisible(state);
}

void CustomDockWidgetBar::setAutoHideEnadled(bool state)
{
    autoHideEnabled = state;
    if(state) {
        autoHideButton->setStyleSheet(s_autoHideEnabledStyle);
    }
    else {
        autoHideButton->setStyleSheet(s_autoHideDisabledStyle);
    }
}

QPoint CustomDockWidgetBar::menuPos() const
{
    QPoint p = menuButton->pos();
    p.ry() += menuButton->height();

    return QPoint(mapToGlobal(p));
}

void CustomDockWidgetBar::mousePressEvent(QMouseEvent *event)
{
    if((event->button() == Qt::LeftButton) && autoHideEnabled) {
        leftMousePressed = true;
    }

    QWidget::mousePressEvent(event);
}

void CustomDockWidgetBar::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        leftMousePressed = false;
    }

    QWidget::mouseReleaseEvent(event);
}

void CustomDockWidgetBar::mouseMoveEvent(QMouseEvent *event)
{
    if(leftMousePressed)
    {
        QDockWidget* dockWidget = static_cast<QDockWidget*>(parentWidget());
        if(dockWidget != nullptr)
        {
            leftMousePressed = false;

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

    QWidget::mouseMoveEvent(event);
}
