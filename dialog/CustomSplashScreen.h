#ifndef CUSTOMSPLASHSCREEN_H
#define CUSTOMSPLASHSCREEN_H

#include <QSplashScreen>

namespace Ui {
class CustomSplashScreen;
}

class CustomSplashScreen : public QSplashScreen
{
    Q_OBJECT

public:
    explicit CustomSplashScreen(QPixmap *map = nullptr);
    ~CustomSplashScreen();

    void setMessage(const QString &message);
    void setProgressValue(const int &val);

private:
    Ui::CustomSplashScreen *ui;
};

#endif // CUSTOMSPLASHSCREEN_H
