#include <ElaApplication.h>

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "../ui/mainwindow.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Leno_" + QLocale(locale).name();
        if (translator.load(":/res/i18n/" + baseName)) {
            QApplication::installTranslator(&translator);
            break;
        }
    }
    eApp->init();
    MainWindow w;
    w.show();
    return QApplication::exec();
}
