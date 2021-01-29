#pragma once

#include <QDockWidget>
#include <QMenu>
#include <QMainWindow>

#include "CustomDockWidgetBar.h"

enum class DockWidgetState
{
	Unknown = -1,
	Docked = 0,    //! DockWidget is docked on MainWindow
	Floating = 1,  //! DockWidget is floating
	Hidden = 2,    //! DockWidget is auto hidden
	Closed = 3,    //! DockWidget is closed by button X
};

class CustomDockWidget : public QDockWidget
{
	Q_OBJECT
public:
    explicit CustomDockWidget();
    ~CustomDockWidget();

    QString windowTitle() const {
            return m_titleWidget->getText();
    }
	void setWindowTitle(const QString& title);

	bool isMinimized() const { return (m_state == DockWidgetState::Hidden); }
	bool isDocked() const { return (m_state == DockWidgetState::Docked); }
	bool isFloating() const { return (m_state == DockWidgetState::Floating); }

	Qt::DockWidgetArea getArea() const { return m_area; }

	DockWidgetState getState() const { return m_state; }
	void setState(DockWidgetState state);

	void closeDockWidget();

	void setWidget(QWidget* widget);
    void removeWidget();

	void setTabifiedDocks(const QList<QDockWidget*>& dockWidgetList);
    const std::vector<CustomDockWidget*>& getTabifiedDocks() const { return m_tabifieds; }
	void clearTabifiedDocks() { m_tabifieds.clear(); }

private:
	void openTitleMenu();
	void autoHideStateToggle();
	void updateDockLocation(Qt::DockWidgetArea area);
	void updateTopLevelState(bool topLevel);
    void removeFromTabifiedDocks(CustomDockWidget* dockWidget);

protected:
	virtual bool event(QEvent* event) override;

signals:
    void signal_pinned(CustomDockWidget* dockWidget);
    void signal_unpinned(CustomDockWidget* dockWidget);
    void signal_docked(CustomDockWidget* dockWidget);
    void signal_undocked(CustomDockWidget* dockWidget);

private slots:
	void slot_menuAction() {}

private:
	Qt::DockWidgetArea m_area;
    CustomDockWidgetBar* m_titleWidget;
	DockWidgetState m_state;
	QVBoxLayout* m_layout;
    std::vector<CustomDockWidget*> m_tabifieds;
    QWidget *contents;
};
