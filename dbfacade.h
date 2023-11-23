#ifndef DBFACADE_H
#define DBFACADE_H

#include <QObject>
#include <QtSql/QtSql>

class QTableView;
class QSqlTableModel;

class DBFacade : public QObject
{
    Q_OBJECT
public:
    DBFacade(QTableView* tags, QObject *parent = nullptr);
    virtual ~DBFacade();

    void addTag(QString id, QString com);
            // добавляет
    void repTag(QString id, QString com);
            // заменяет
    void remTag(QString id);
            // удаляет
    bool checkTagID(QString id);
            // проверяет наличие id в базе данных
    void expTable();

protected: 
    QSqlDatabase m_db;
          // база данных
    QSqlQuery *m_query;
          // запрос к базе
    QSqlRecord m_rec;
          // строка таблицы (ответ на запрос)
    QSqlTableModel *m_tagsModel;
          // модель таблицы
};

#endif // DBFACADE_H
