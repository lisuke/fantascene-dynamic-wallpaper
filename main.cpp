#include <QObject>

#include "application.h"
#include "wallpaper.h"
#include "dbuswallpaperservice.h"
#include "settings.h"
#include <malloc.h>
#include "settingwindow.h"
#include <DMainWindow>
#include <QProcess>
#include <QThread>
#include <QDesktopWidget>
#include <QDebug>
#include <QCoreApplication>
#include <DDesktopEntry>
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
void cpToTmp()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/deepin-dreamscene/";
    QDir dir(path);
    if (!dir.exists()) {
        QDir dir1;
        dir1.mkpath(path);
    }
//    QFile::copy(QCoreApplication::applicationDirPath()+"/dynamicWallPaper.desktop",path+"dynamicWallPaper.desktop");
////    if(!QFileInfo(path+"dynamicWallPaper.desktop").isFile())
//    QProcess::execute("cp dynamicWallPaper.desktop "+path);

////    if(!QFileInfo(path+"libdde-file-manager.so.1.8.2").isFile())
//    QProcess::execute("cp libdde-file-manager.so.1.8.2 "+path);

////    if(!QFileInfo(path+"libdde-file-manager.so.1.8").isFile())
//    QProcess::execute("cp libdde-file-manager.so.1.8 "+path);

////    if(!QFileInfo(path+"libdde-file-manager.so.1").isFile())
//    QProcess::execute("cp libdde-file-manager.so.1 "+path);

////    if(!QFileInfo(path+"libdde-file-manager.so").isFile())
//    QProcess::execute("cp libdde-file-manager.so "+path);

////    if(!QFileInfo(path+"dde-desktop").isFile())
//    QProcess::execute("cp dde-desktop "+path);

////    if(!QFileInfo(path+"deepin-dreamscene").isFile())
//    QProcess::execute("cp deepin-dreamscene "+path);

////    if(!QFileInfo(path+"config.ini").isFile())
//    QProcess::execute("cp config.ini "+path);

////    if(!QFileInfo(path+"9.mp4").isFile())
//    QProcess::execute("cp 9.mp4 "+path);

////    QProcess::execute("cp start "+path);
////    QProcess::execute("cp start-min "+path);

    qDebug() << QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
//        qDebug()<<QStandardPaths::HomeLocation;
    qDebug() << QCoreApplication::applicationDirPath() + "/dynamicWallPaper.desktop";
}
int main(int argc, char *argv[])
{
    cpToTmp();
//    QString path=QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/deepin-dreamscene/";
    QString path = "/opt/deepin-dreamscene-ui/";

    mallopt(M_ARENA_MAX, 1);

    Application a(argc, argv);
    //打印当前路径
    qDebug() << QCoreApplication::applicationDirPath();

    QThread *th = QThread::create([ = ]() {
        QProcess::execute("killall dde-desktop");
        qDebug() << "killall dde-desktop";
        qDebug() << "loading new dde-desktop";
        if (QFileInfo(path + "/dde-desktop").isFile()) {
            QProcess process;
            process.execute(path + "dde-desktop");
        } else {
            QProcess process;
            process.execute("./dde-desktop");
        }
    });
    th->start();

    a.setTheme("light");

    setlocale(LC_NUMERIC, "C");

    if (a.setSingleInstance("deepin-dreamscene")) {
        a.setApplicationName(QObject::tr("Deepin DreamScene"));
        a.setApplicationVersion("Version 0.1");

        DMainWindow *mainwindw = new DMainWindow();
        settingWindow *window = new settingWindow(mainwindw, mainwindw);
        mainwindw->setCentralWidget(window);
        int index = 0;
        for (const QString &arg : qApp->arguments()) {
            if (arg == "min") {
                index++;
            }
        }
        if (index == 0) {
            mainwindw->show();
        }

        Wallpaper *w = new Wallpaper(window->getCurrentPath(), window->getCurrentNumber());
        qDebug() << qApp->desktop()->screenNumber();
        qDebug() << qApp->desktop()->primaryScreen();
        qDebug() << qApp->desktop()->screenCount();

        DBusWallpaperService *dbusInter = new DBusWallpaperService(w);
        Q_UNUSED(dbusInter);

        QDBusConnection::sessionBus().registerService("com.deepin.dde.DreamScene");
        QDBusConnection::sessionBus().registerObject("/com/deepin/dde/DreamScene", "com.deepin.dde.DreamScene", w);

        QString envName("DDE_SESSION_PROCESS_COOKIE_ID");

        QByteArray cookie = qgetenv(envName.toUtf8().data());
        qunsetenv(envName.toUtf8().data());

        if (!cookie.isEmpty()) {
            QDBusInterface iface("com.deepin.SessionManager",
                                 "/com/deepin/SessionManager",
                                 "com.deepin.SessionManager",
                                 QDBusConnection::sessionBus());
            iface.asyncCall("Register", QString(cookie));
        }
//        qDebug() << QCoreApplication::applicationDirPath() + "dynamicWallPaper.desktop";
//        DDesktopEntry desktop(path + "dynamicWallPaper.desktop");
//        QString name = desktop.name();
//        QString name1 = desktop.name();
//        QString Exec;
//        desktop.setStringValue(path + "start", "Exec");
//        desktop.save();
    }

    return a.exec();
}

