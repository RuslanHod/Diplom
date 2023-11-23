#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <QByteArray>
#include <QTextCodec>
#include <QMessageBox>

namespace Ui
{
    class MainWindow;
}

class DBFacade;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_WriteDataOnCard_clicked();
    void on_ReadDataOnCard_clicked();
    void on_DevCon_clicked();
    void on_DevDis_clicked();
    void on_tagAdd_clicked();
    void on_tagRem_clicked();
    void on_ExpTagTab_clicked();
    void ReadData();
    bool CheckDigID(QString id);

private:
    Ui::MainWindow *m_ui;
    DBFacade *m_db;
    QSerialPort *thisPort = nullptr;
    QTextCodec *codecC = QTextCodec::codecForName( "CP1251" );
    QTextCodec *codecU = QTextCodec::codecForName( "UTF-8" );
};

#endif // MAINWINDOW_H
