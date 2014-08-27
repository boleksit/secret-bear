#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QtNetwork>
#include <QMainWindow>
#include <QFileDialog>
#include <QTextEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;

public slots:
    void chooseInput1();
    void chooseInput2();
    void chooseInput3();
    void chooseInput4();
    void chooseInput5();
    void chooseOutput1();
    void chooseOutput2();
    void chooseOutput6();
    void encode();
    void decode();
    void compare();
    void upload();
    void uploaded(QNetworkReply *reply);
    void uploadProgress(qint64 sent, qint64 total);
    void download();
    void downloaded(QNetworkReply *reply);
    void downloadProgress(qint64 recieved, qint64 total);
};

#endif // MAINWINDOW_H
