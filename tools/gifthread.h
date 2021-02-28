#ifndef GIFMASK_H
#define GIFMASK_H

#include <QThread>
#include <QScreen>
#include <QApplication>
#include <QPixmap>

#include "gif.h"

class gifThread : public QThread
{
    Q_OBJECT

public:
    explicit gifThread(QObject *parent = nullptr);
    void StartWork(const QString &fileName, const QRect &rectGif);
    void StopWork();

protected:
    void run();

private:
    int recordFps;
    QString fileName;
    QRect rectGif;

    Gif gifHandle;
    Gif::GifWriter *gifWriter;

};

#endif // GIFMASK_H
