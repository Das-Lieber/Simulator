#include "CustomDockWidgetTabButton.h"

CustomDockWidgetTabButton::CustomDockWidgetTabButton(const QString& text, Qt::DockWidgetArea area)
    : QPushButton(text, nullptr)
	, m_action(nullptr)
    , m_area(area)
{
	setToolTip(text);

	m_orientation = areaToOrientation(area);

	int fw = fontMetrics().width(text)+12;

	fw = (fw < 15) ? 15 : fw;
	fw = (fw > 120) ? 121 : fw;

	if(m_orientation == Qt::Vertical) {
        setFixedSize(25, fw+10);
	}
	else if(m_orientation == Qt::Horizontal) {
        setFixedSize(fw+10, 25);
	}
}

CustomDockWidgetTabButton::~CustomDockWidgetTabButton()
{	
}

void CustomDockWidgetTabButton::setText_(const QString& text)
{
	int aw = (m_orientation == Qt::Horizontal) ? width() - 4 : height() - 4;

	QFontMetrics fm = fontMetrics();
	if(aw < fm.width(text))
	{
		QString str;

		// Need to cut the text
		for(int i = 0; i < text.size(); i++)
		{
			str += text.at(i);

			if(fm.width(str + ".......") > aw)
				break;
		}

		setText(str + "...");
	}
	else
	{
		setText(text);
	}
}

QStyleOptionButton CustomDockWidgetTabButton::getStyleOption() const
{
    QStyleOptionButton opt;
    opt.initFrom(this);

    if(m_orientation == Qt::Vertical)
    {
        QSize size = opt.rect.size();
        size.transpose();
        opt.rect.setSize(size);
    }

    opt.features = QStyleOptionButton::None;

    if(isFlat()) {
        opt.features |= QStyleOptionButton::Flat;
    }
    if(menu()) {
        opt.features |= QStyleOptionButton::HasMenu;
    }

    if(autoDefault() || isDefault()) {
        opt.features |= QStyleOptionButton::AutoDefaultButton;
    }

    if(isDefault()) {
        opt.features |= QStyleOptionButton::DefaultButton;
    }

    if(isDown() || (menu() && menu()->isVisible())) {
        opt.state |= QStyle::State_Sunken;
    }

    if(isChecked()) {
        opt.state |= QStyle::State_On;
    }

    if(!isFlat() && !isDown()) {
        opt.state |= QStyle::State_Raised;
    }

    opt.text = text();
    opt.icon = icon();
    opt.iconSize = iconSize();
    return opt;
}

void CustomDockWidgetTabButton::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

	QStylePainter painter(this);

	switch(m_orientation)
	{
	case Qt::Vertical:
		{
			painter.rotate(90);
			painter.translate(0, -width());
            break;
		}		
	}

	painter.drawControl(QStyle::CE_PushButton, getStyleOption());
}

void CustomDockWidgetTabButton::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
	setText_(text());
}

QSize CustomDockWidgetTabButton::sizeHint() const
{
    QSize size = QPushButton::sizeHint();
    if(m_orientation == Qt::Vertical) {
        size.transpose();
    }
    return size;
}
