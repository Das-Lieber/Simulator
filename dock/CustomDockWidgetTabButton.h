#pragma once

#include <QPushButton>
#include <QStyleOptionButton>
#include <QStylePainter>
#include <QMenu>

inline Qt::Orientation areaToOrientation(Qt::DockWidgetArea area)
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

class CustomDockWidgetTabButton : public QPushButton
{
	Q_OBJECT
public:
    CustomDockWidgetTabButton(const QString& text, Qt::DockWidgetArea area);
    ~CustomDockWidgetTabButton();

	void setAction(QAction* action) { m_action = action; }
	QAction* getAction() const { return m_action; }

private:
	void setText_(const QString& text);
	QStyleOptionButton getStyleOption() const;

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual QSize sizeHint() const override;

private:
	QAction* m_action;
	Qt::DockWidgetArea m_area;
	Qt::Orientation m_orientation;
};
