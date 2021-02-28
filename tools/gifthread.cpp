#include "gifthread.h"

gifThread::gifThread(QObject *parent)
    : QThread(parent)
    , recordFps(120)
    , gifWriter(0)
{
}

void gifThread::StartWork(const QString &fileName, const QRect &rectGif)
{
    this->fileName = fileName;
    this->rectGif = rectGif;
}

void gifThread::StopWork()
{
    requestInterruption();
}

void gifThread::run()
{
    if (0 != gifWriter) {
        delete gifWriter;
        gifWriter = 0;
    }

    gifWriter = new Gif::GifWriter;
    bool bOk = gifHandle.GifBegin(gifWriter,fileName.toLocal8Bit().data(),rectGif.width(),rectGif.height(),1000/recordFps);
    if (!bOk) {
        delete gifWriter;
        gifWriter = 0;
        return;
    }

    while(!isInterruptionRequested())
    {
        QScreen *screen = qApp->primaryScreen();
        QPixmap pix = screen->grabWindow(0, rectGif.x(), rectGif.y(), rectGif.width(), rectGif.height());
        QImage image = pix.toImage().convertToFormat(QImage::Format_RGBA8888);
        gifHandle.GifWriteFrame(gifWriter, image.bits(), rectGif.width(), rectGif.height(), 1000/recordFps);

        msleep(1000 / recordFps);
    }

    gifHandle.GifEnd(gifWriter);

    delete gifWriter;
    gifWriter = 0;
}
