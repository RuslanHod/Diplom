#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbfacade.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    thisPort(new QSerialPort(this))
{
    m_ui->setupUi(this);
    m_db = new DBFacade(m_ui->tagView, this);
    //Считываем активные последовательные порты
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    //Отправляем эти данные в comboBox
    m_ui->DevComboBox->addItem(info.portName());
    //По сигналу readyRead вызывает функцию ReadData
    connect(thisPort, SIGNAL(readyRead()),this,SLOT(ReadData()));
}


MainWindow::~MainWindow()
{
    delete m_ui;
}

//Функция, отвечающая за подключение к устройству
void MainWindow::on_DevCon_clicked()
{
    //Выбираем имя нашего последовательного порта в выпадающем списке
    if (thisPort->portName() != m_ui->DevComboBox->currentText())
    {
          thisPort->close();
          thisPort->setPortName(m_ui->DevComboBox->currentText());
    }
    //Устанавливаем настройки порта
    thisPort->setBaudRate(QSerialPort::Baud4800);
    thisPort->setDataBits(QSerialPort::Data8);
    thisPort->setParity(QSerialPort::NoParity);
    thisPort->setStopBits(QSerialPort::OneStop);
    thisPort->setFlowControl(QSerialPort::NoFlowControl);
    thisPort->open(QSerialPort::ReadWrite);
    //Если порт открыт
    if (thisPort->isOpen())
    {
        //Вывод текста в всплывающее окно
        QMessageBox::information(nullptr, "Внимание", "Соединение с устройством установлено");
    }
    else //или
    {
        //Вывод текста в всплывающее окно
        QMessageBox::information(nullptr, "Внимание", "Соединение с устройством не установлено");
    }
}

//Функция, отвечающая за отключение от устройства
void MainWindow::on_DevDis_clicked()
{
    //Если порт открыт
    if(thisPort->isOpen())
    {
        //Закрываем
        thisPort->close();
        // Вывод текста в MessageBox
        QMessageBox::information(nullptr, "Внимание", "Соединение с устройством разорвано");
    }
    else //или
    {
        //Вывод текста в MessageBox
        QMessageBox::information(nullptr, "Внимание", "Соединение с устройством не установлено");
    }
}

//Функция, отвечающая за отправку запроса на запись данных на метку
void MainWindow::on_WriteDataOnCard_clicked()
{
    m_ui->DevTagBrowser->clear();
    //Если порт открыт
    if(thisPort->isOpen())
    {
        if (m_ui->tagID->text().isEmpty() || m_ui->tagCom->toPlainText().isEmpty())
        {
            //Вызов окна с информацией
            QMessageBox::information(nullptr, "Внимание", "Поле инвентарного номера и/или поле описания пусто->заполните их");
            return;
        }
        if (m_ui->tagID->text().length() > 19)
        {
            //Вызов окна с информацией
            QMessageBox::information(nullptr, "Внимание", "Инвентарный номер не может быть больше 19 цифр");
            return;
        }
        if (!CheckDigID(m_ui->tagID->text()))
        {
            QMessageBox::information(nullptr, "Внимание", "Инвентарный номер должен состоять только из цифр");
            return;
        }
        //Блокировка метки
        thisPort->write("b");
        thisPort->write("\xd");
        //Создаем и обнуляем
        QByteArray dataOutI = nullptr;
        QByteArray dataOutC = nullptr;
        //Присваиваем dataOutI значение полученные из tagID
        //Присваиваем dataOutC значение полученные из tagCom
        dataOutI = "i:"+m_ui->tagID->text().toLocal8Bit();
        dataOutC = "c:"+m_ui->tagCom->toPlainText().toLocal8Bit();
        //Записываем данные в порт
        for (int i = 0; i < dataOutI.count(); ++i)
        {
            thisPort->putChar(dataOutI[i]);
        }
        thisPort->write("\xd");

        //Записываем данные в порт
        for (int i = 0; i < dataOutC.count(); ++i)
        {
            thisPort->putChar(dataOutC[i]);
        }
        thisPort->write("\xd");

        //Разблокировка метки
        thisPort->write("ub");
        thisPort->write("\xd");
    }
    else //или
    {
        //Вывод текста в MessageBox
        QMessageBox::information(nullptr, "Внимание", "Соединение с устройством не установлено");
    }
}

//Функция, отвечающая за отправку запроса на чтение данных с метки
void MainWindow::on_ReadDataOnCard_clicked()
{
    m_ui->DevTagBrowser->clear();
    //Если порт открыт
    if(thisPort->isOpen())
    {
        //Блокируем
        thisPort->write("b");
        thisPort->write("\xd");
        //Отправляем на устройство i\xd
        thisPort->write("i");
        thisPort->write("\xd");
        thisPort->write("c");
        thisPort->write("\xd");
        //Разблокируем
        thisPort->write("ub");
        thisPort->write("\xd");
    }
    else //или
    {
        //Вывод текста в MessageBox
        QMessageBox::information(nullptr, "Внимание", "Соединение с устройством не установлено");
    }
}

//Функция, отвечающая за запись данных в базу данных
void MainWindow::on_tagAdd_clicked()
{
    //Если поля tagID и tagCom пусто, то return
    if (m_ui->tagID->text().isEmpty() || m_ui->tagCom->toPlainText().isEmpty())
    {
        //Вызов окна с информацией
        QMessageBox::information(nullptr, "Внимание", "Поле инвентарного номера и/или поле описания пусто->заполните их");
        return;
    }
    if (m_ui->tagID->text().length() > 19)
    {
        //Вызов окна с информацией
        QMessageBox::information(nullptr, "Внимание", "Инвентарный номер не может быть больше 19 цифр");
        return;
    }
    if (!CheckDigID(m_ui->tagID->text()))
    {
        QMessageBox::information(nullptr, "Внимание", "Инвентарный номер должен состоять только из цифр");
        return;
    }
    //Если функция checkTagID вернул true
    if (m_db->checkTagID(m_ui->tagID->text()))
    {
        //Создаем оповещение если строка с введенным инвентарным номером уже существует
        //и предлагает перезаписать строку
        int n = QMessageBox::warning(nullptr,
                                     "Внимание",
                                     "Строка с введенным инвентарным номером уже существует."
                                     "\nЖелаете ли вы перезаписать строку?",
                                     "Да",
                                     "Нет",
                                     QString(),
                                     0,
                                     1
                                    );
        //Если нажали да, то вызываем функцию repTag и передаем данные из полей и return
        if(!n)
        {
            m_db->repTag(m_ui->tagID->text(), m_ui->tagCom->toPlainText());
            return;
        }
        //Если нажали нет, то return
        if(n)
        {
            return;
        }
    }
    //Вызываем функцию addTag и передаем ей параметры полей
    m_db->addTag(m_ui->tagID->text(), m_ui->tagCom->toPlainText());
}

//Функция, отвечающая за удаление данных из базы данных
void MainWindow::on_tagRem_clicked()
{
    if (m_ui->tagID->text().isEmpty())
    {
        QMessageBox::information(nullptr, "Внимание", "Поле инвентарного номера пусто->заполните его");
        //Если поле tagID пусто, то return
        return;
    }
    //Если функция checkTagID вернул false
    if (!m_db->checkTagID(m_ui->tagID->text()))
    {
        QMessageBox::information(nullptr, "Внимание", "Строки с данным инвентарным номером нет");
        return;
    }
    //Вызываем функцию remTag и передаем ей параметр поля
    m_db->remTag(m_ui->tagID->text());
}

//Функция, отвечающая за вывод даных из базы данных
void MainWindow::on_ExpTagTab_clicked()
{
    m_db->expTable();
    QMessageBox::information(nullptr, "Внимание", "Файл создан");
}

//Функция, отвечающая за проверку введенного инвентарного номера на наличие букв
bool MainWindow::CheckDigID(QString id)
{
    for (int i = 0; i < id.count(); ++i)
    {
        if (!id[i].isDigit())
            return false;
    }
    return true;
}

//Функция, отвечающая за чтение данных с порта
void MainWindow::ReadData()
{
    //Создаем и обнуляем
    QByteArray dataIn = nullptr;
    //Присваиваем dataIn данные полученные с порта
    dataIn = thisPort->readAll();
    //Создаем переменую strf и перекодируем её из cp1251 в юникод
    QString strf = codecC->toUnicode(dataIn);
    //Переменой dataIn присваиваем значения строки strf, перекодируя из юникода в utf-8
    dataIn = codecU->fromUnicode(strf);
    //Выводим на экран данные полученные с порта
    m_ui->DevTagBrowser->insertPlainText(dataIn);
}
