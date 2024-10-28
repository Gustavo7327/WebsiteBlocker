#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QListView>
#include <QStringListModel>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSites();
}
MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_addButton_clicked()
{
    QInputDialog dialog(this);
    dialog.setWindowTitle("Adicionar Site Para Bloqueio");
    dialog.setLabelText("Insira o domínio do site que deseja bloquear");

    QFont font = dialog.font();
    font.setPointSize(12);
    dialog.setFont(font);

    dialog.exec();
    QString newSite = dialog.textValue().trimmed();

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

    qDebug() << "Site adicionado com sucesso:" << newSite;

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Informação");
    msgBox.setText("Site " + newSite + " adicionado com sucesso!");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    font.setPointSize(12);
    msgBox.setFont(font);
    msgBox.exec();
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
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Informação");

    QFont font = msgBox.font();
    font.setPointSize(12);
    msgBox.setFont(font);

    QStringListModel *model = qobject_cast<QStringListModel*>(ui->listView->model());

    if (model) {
        QModelIndex index = ui->listView->currentIndex();

        if (index.isValid()) {
            QString itemToRemove = "127.0.0.1 " + model->data(index, Qt::DisplayRole).toString();

            msgBox.setText("Tem certeza que deseja remover " + itemToRemove.replace("127.0.0.1 ", "") + "?");
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

            if(msgBox.exec() == QMessageBox::Cancel){
                return;
            }

            model->removeRow(index.row());
            removeItemFromFile(itemToRemove);
            updateHostsFile(itemToRemove, true);

            qDebug() << "Item removido com sucesso.";

            msgBox.setText(itemToRemove.replace("127.0.0.1 ", "") + " removido com sucesso!");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStandardButtons(QMessageBox::Ok);

        } else {
            qDebug() << "Nenhum item selecionado.";

            msgBox.setText("Selecione um item para removê-lo");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
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



void MainWindow::on_quitButton_clicked()
{
    QApplication::quit();
}

