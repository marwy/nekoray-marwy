#include "dialog_app_proxy.h"
#include "ui_dialog_app_proxy.h"

#include "main/NekoGui.hpp"
#include <QFileDialog>
#include <QStringList>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>
#include <QApplication>

DialogAppProxy::DialogAppProxy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAppProxy)
{
    ui->setupUi(this);
    loadAppList();
}

DialogAppProxy::~DialogAppProxy()
{
    delete ui;
}

void DialogAppProxy::on_addButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Select Application"), "", tr("Executable Files (*.exe);;All Files (*)"));
    if (!filePath.isEmpty()) {
        ui->appListWidget->addItem(filePath);
    }
}

void DialogAppProxy::on_removeButton_clicked()
{
    qDeleteAll(ui->appListWidget->selectedItems());
}

void DialogAppProxy::on_launchButton_clicked()
{
    auto selectedItems = ui->appListWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select an application to launch."));
        return;
    }

    QString appPath = selectedItems.first()->text();

    // Prepare proxychains config
    QString configContent = "[ProxyList]\nsocks5  ";
    configContent += NekoGui::dataStore->inbound_address;
    configContent += " " + QString::number(NekoGui::dataStore->inbound_socks_port);

    QString configPath = QDir::tempPath() + "/proxychains.conf";
    QFile configFile(configPath);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not write temporary config file."));
        return;
    }
    QTextStream out(&configFile);
    out << configContent;
    configFile.close();

    // Launch process
    QProcess *process = new QProcess(this);
    QString proxychainsPath = QApplication::applicationDirPath() + "/proxychains.exe";

    if (!QFile::exists(proxychainsPath)) {
        QMessageBox::critical(this, tr("Error"), tr("proxychains.exe not found. Please place it in the application directory."));
        return;
    }

    QStringList arguments;
    arguments << "-f" << configPath << appPath;
    
    process->startDetached(proxychainsPath, arguments);
}

void DialogAppProxy::accept()
{
    saveAppList();
    QDialog::accept();
}

void DialogAppProxy::loadAppList()
{
    QStringList appList = NekoGui::dataStore->app_proxy_list.split("\n", Qt::SkipEmptyParts);
    ui->appListWidget->addItems(appList);
}

void DialogAppProxy::saveAppList()
{
    QStringList appList;
    for(int i = 0; i < ui->appListWidget->count(); ++i)
    {
        appList.append(ui->appListWidget->item(i)->text());
    }
    NekoGui::dataStore->app_proxy_list = appList.join("\n");
} 