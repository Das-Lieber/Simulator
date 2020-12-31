#ifndef CUSTOMDOCKWIDGET_H
#define CUSTOMDOCKWIDGET_H

#include <QDockWidget>
#include <QMenu>
#include <QMainWindow>

#include "CustomDockWidgetBar.h"

enum DockWidgetState
{
    Unknown = -1,
    Docked = 0,
    Floating = 1,
    Hidden = 2,
    Closed = 3,
};

class CustomDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit CustomDockWidget(QWidget *parent = nullptr);

    QString windowTitle() const {
        return titleWidget->getText();
    }
    void setWindowTitle(const QString& title) {
        QString tmp = title.isEmpty() ? "Noname" : title;

        titleWidget->setText(tmp);
        QDockWidget::setWindowTitle(tmp);
    }

    bool isMinimized() const { return (dockWidgetState == DockWidgetState::Hidden); }
    bool isDocked() const { return (dockWidgetState == DockWidgetState::Docked); }
    bool isFloating() const { return (dockWidgetState == DockWidgetState::Floating); }

    Qt::DockWidgetArea getArea() const { return m_area; }

    DockWidgetState getState() const { return dockWidgetState; }

    void setWidget(QWidget *widget);
    void setDockWidgetState(DockWidgetState state);

    const std::vector<CustomDockWidget*>& getTabifiedDocks() const { return m_tabifieds; }
    void setTabifiedDocks(const QList<QDockWidget*>& dockWidgetList);
    void clearTabifiedDocks() { m_tabifieds.clear(); }

protected:
    virtual bool event(QEvent* event) override;

private:
    CustomDockWidgetBar *titleWidget;
    QVBoxLayout* globalLayout;
    DockWidgetState dockWidgetState;
    std::vector<CustomDockWidget*> m_tabifieds;
    Qt::DockWidgetArea m_area;

    void updateDockLocation(Qt::DockWidgetArea area);
    void updateTopLevelState(bool topLevel);
    void removeFromTabifiedDocks(CustomDockWidget* dockWidget);

signals:
    void signal_pinned(CustomDockWidget* dockWidget);
    void signal_unpinned(CustomDockWidget* dockWidget);
    void signal_docked(CustomDockWidget* dockWidget);
    void signal_undocked(CustomDockWidget* dockWidget);

private slots:
    void slot_menuAction() {}
};

#endif // CUSTOMDOCKWIDGET_H
