#include "dbfacade.h"

#include <QTableView>

DBFacade::DBFacade(QTableView* tags, QObject *parent)
  : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    //Активизируем драйвер
    m_db.setDatabaseName("DataBaseForDProject");
    //Устанавливаем имя даты базы
    m_db.open();
    //Открываем базу данных
    m_query = new QSqlQuery(m_db);
    //Создаем объект класса QSqlQuery для использования команд SQL
    if (false == m_db.tables().contains("tags"))
        m_query->exec("create table tags (Инвентарный_номер integer primary key, Описание varchar)");
        //Передаем команду на создание таблицы с именем tags со столбцами Инвентарный_номер, Описание
    m_tagsModel = new QSqlTableModel(this, m_db);
    //Класс QSqlTableModel позволяет отображать данные в табличной и иерархической форме
    m_tagsModel->setTable("tags");
    //Устанавливает таблицу базы данных, с которой работает модель
    m_tagsModel->select();
    //Вызов select() производит заполнение данными.
    m_tagsModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    //Производит запись после того, как пользователь перейдет к другой ячейке таблицы
    tags->setModel(m_tagsModel);
    //Устанавливает модель
    tags->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //Блокирует ячейки таблицы из экрана приложения
}


DBFacade::~DBFacade()
{
    delete m_query;
}

//Функция, отвечающаяя за довление строк
void DBFacade::addTag(QString id, QString com)
{
    m_query->first();
    m_query->prepare("INSERT INTO tags (Инвентарный_номер, Описание) VALUES ('"+id+"','"+com+"')");
    //Подготавливает SQL запрос к выполнению
    m_query->exec("INSERT INTO tags (Инвентарный_номер, Описание) VALUES ('"+id+"','"+com+"')");
    //Отправляем запрос
    m_tagsModel->select();
    //Заполнение данными
}

//Функция, отвечающаяя за замену строк
void DBFacade::repTag(QString id, QString com)
{
    m_query->prepare("REPLACE INTO tags (Инвентарный_номер, Описание) VALUES ('"+id+"','"+com+"')");
    //Подготавливает SQL запрос к выполнению
    m_query->exec("REPLACE INTO tags (Инвентарный_номер, Описание) VALUES ('"+id+"','"+com+"')");
    //Отправляем запрос
    m_tagsModel->select();
}

//Функция, отвечающаяя за проверку совпадающих идентификаторов
bool DBFacade::checkTagID(QString id)
{
    m_query->exec(("SELECT Инвентарный_номер FROM tags WHERE Инвентарный_номер =")+ id);
    {
        while (m_query->next())
        {
            return true;
        }
    }
    return false;
}

//Функция, отвечающая за удаляление строки
void DBFacade::remTag(QString id)
{
    m_query->exec(("DELETE FROM tags WHERE Инвентарный_номер = ") + id);
    //Удаляем из таблицы строку с нужным id
    m_tagsModel->select();
}

void DBFacade::expTable()
{
    QString textData;
    int rows = m_tagsModel->rowCount();
    int columns = m_tagsModel->columnCount();

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            textData += m_tagsModel->data(m_tagsModel->index(i,j)).toString();
            textData += ";";
        }
        textData += "\n";
    }

    QFile csvFile("DataBaseTag.csv");
    if(csvFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream out(&csvFile);
        out << textData;
        csvFile.close();
    }
}
