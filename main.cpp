#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
#ifdef Q_OS_UNIX 
    if (geteuid() != 0) {
        QMessageBox::critical(nullptr, "Erro", "Este aplicativo requer privilégios administrativos. Por favor, execute com sudo.");
        return 1;
    }
#endif

    return a.exec();
}
