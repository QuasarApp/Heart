#ifndef DBOBJECT_H
#define DBOBJECT_H
#include <QVariantMap>
#include <dbtablebase.h>
#include "abstractdata.h"
#include "clientprotocol_global.h"

class QSqlQuery;

namespace ClientProtocol {

/**
 * @brief The DBObject class
 */
class CLIENTPROTOCOLSHARED_EXPORT DBObject : public AbstractData
{
public:
    /**
     * @brief DBObject
     */
    DBObject(const QString& tableName);
    ~DBObject() override;


    /**
     * @brief saveQuery
     * @param query update value for sql query
     * @return true if all good
     */
    virtual bool saveQuery(QSqlQuery *query) const;

    /**
     * @brief selectQuery
     * @param query update value
     * @return return true if all good
     */
    virtual bool selectQuery(QSqlQuery *query);

    /**
     * @brief deleteQuery
     * @param query delete value
     * @return true if all good
     */
    virtual bool deleteQuery(QSqlQuery *query) const;

    /**
     * @brief getId
     * @return id of objcet
     */
    int getId() const;

    /**
     * @brief getId
     * @return id of objcet
     */
    void setId(int);

    /**
     * @brief clear
     */
    virtual void clear();

    /**
     * @brief getTableStruct
     * @return
     */
    DbTableBase getTableStruct() const;

    /**
     * @brief setTableStruct
     * @param tableStruct
     */
    void setTableStruct(const DbTableBase &tableStruct);

protected:

    /**
     * @brief generateHeaderOfQuery - generate list of columns header for update
     * @param retQuery return value
     * @return true if all good
     */
    virtual bool generateHeaderOfQuery(QString &retQuery) const;

    /**
     * @brief generateSourceOfQuery - enerate list of columns header for update
     * @param retQuery return value
     * @param retBindValue list of bind value, after invoce of this method need invoce
     * @return
     */
    virtual bool generateSourceOfQuery(QString &retQuery,
                                       QList<QPair<QString, QVariant>>& retBindValue) const;

    /**
     * @brief getBaseQueryString private implementation of getQueryMethods
     * @param query
     * @return true if all good
     */
    virtual bool getBaseQueryString(QString queryString, QSqlQuery *query) const;

    QVariantMap _dataTable;
    DbTableBase _tableStruct;


    //// StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
    QVariantMap &fromVariantMap(QVariantMap &map) override;
    QVariantMap &toVariantmap(QVariantMap &map) const override;

    /**
     * @brief fromQuery
     * @param query
     * @return
     */
    virtual bool exec(QSqlQuery *query) const;

    //// AbstractData interface
    bool isValid() const override;

};
}


#endif // DBOBJECT_H
