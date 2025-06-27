#ifndef DIALOG_APP_PROXY_H
#define DIALOG_APP_PROXY_H

#include <QDialog>

namespace Ui {
class DialogAppProxy;
}

class DialogAppProxy : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAppProxy(QWidget *parent = nullptr);
    ~DialogAppProxy();

private slots:
    void on_addButton_clicked();
    void on_removeButton_clicked();
    void on_launchButton_clicked();
    void accept() override;

private:
    Ui::DialogAppProxy *ui;
    void loadAppList();
    void saveAppList();
};

#endif // DIALOG_APP_PROXY_H 