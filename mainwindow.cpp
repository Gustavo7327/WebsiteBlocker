#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QListView>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->siteInput, &QLineEdit::textChanged, [=]{ style()->polish(ui->siteInput); });
    loadSites();
}
MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_addButton_clicked()
{
    QString newSite = ui->siteInput->text().trimmed();

    if (newSite.isEmpty()) {
        qDebug() << "O campo de entrada está vazio.";
        return;
    }

    QFile file(QCoreApplication::applicationDirPath() + "/sites.txt");

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "Não foi possível abrir o arquivo:" << file.errorString();
        return;
    }


    QTextStream out(&file);
    QString siteAdd = "127.0.0.1 " + newSite + "\n";
    out << siteAdd;
    updateHostsFile(siteAdd, false);

    file.close();

    QStringListModel *model = qobject_cast<QStringListModel*>(ui->listView->model());

    if (model) {
        QStringList sites = model->stringList();
        sites << newSite;
        model->setStringList(sites);
    }

    ui->siteInput->clear();
    qDebug() << "Site adicionado com sucesso:" << newSite;
}



void MainWindow::loadSites()
{
    QFile file(QCoreApplication::applicationDirPath() + "/sites.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Não foi possível abrir o arquivo:" << file.errorString();
        return;
    }

    QStringList sites;

    QTextStream in(&file);

    while (!in.atEnd()) {

        QString line = in.readLine().replace("127.0.0.1", "").trimmed();

        if (!line.isEmpty()) {
            sites << line;
        }
    }

    file.close();

    QStringListModel *model = new QStringListModel(this);
    model->setStringList(sites);

    ui->listView->setModel(model);
}



void MainWindow::removeItemFromFile(const QString &item)
{
    QFile file(QCoreApplication::applicationDirPath() + "/sites.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Não foi possível abrir o arquivo:" << file.errorString();
        return;
    }

    QStringList lines;
    QTextStream in(&file);

    while (!in.atEnd()) {

        QString line = in.readLine().trimmed();

        if (!line.isEmpty() && line != item) {
            lines << line;
        }
    }

    file.close();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Não foi possível abrir o arquivo para escrita:" << file.errorString();
        return;
    }

    QTextStream out(&file);

    for (const QString &line : lines) {
        out << line << "\n";
    }

    file.close();
}



void MainWindow::on_removeButton_clicked()
{
    QStringListModel *model = qobject_cast<QStringListModel*>(ui->listView->model());

    if (model) {
        QModelIndex index = ui->listView->currentIndex();

        if (index.isValid()) {
            QString itemToRemove = "127.0.0.1 " + model->data(index, Qt::DisplayRole).toString();
            model->removeRow(index.row());
            removeItemFromFile(itemToRemove);
            updateHostsFile(itemToRemove, true);
            qDebug() << "Item removido com sucesso.";

        } else {
            qDebug() << "Nenhum item selecionado.";
        }
    }
}



void MainWindow::updateHostsFile(const QString &item, bool remove)
{
    QFile file("/etc/hosts");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Não foi possível abrir o arquivo /etc/hosts:" << file.errorString();
        return;
    }

    QTextStream in(&file);


    QStringList lines;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (!line.isEmpty() && line != item) {
            lines << line;
        }
    }

    file.close();

    if(remove){
        lines.removeAll(item);
    } else{
        lines << item;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qDebug() << "Não foi possível abrir o arquivo /etc/hosts para escrita:" << file.errorString();
        file.close();
        return;
    }

    QTextStream out(&file);

    for (const QString &line : lines) {
        out << line << "\n";
    }

    file.close();
}


