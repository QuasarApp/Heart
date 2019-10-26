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
     * @brief getSaveQueryString
     * @param query update value for sql query
     * @return true if all good
     */
    virtual bool getSaveQueryString(QSqlQuery *query) const;

    /**
     * @brief getSelectQueryString
     * @param query update value
     * @return return true if all good
     */
    virtual bool getSelectQueryString(QSqlQuery *query) const;

    /**
     * @brief getDeleteQueryString
     * @param query update value
     * @return true if all good
     */
    virtual bool getDeleteQueryString(QSqlQuery *query) const;

    /**
     * @brief getId
     * @return id of objcet
     */
    int getId() const;

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
    int _id = -1;


    //// StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;
    QVariantMap &fromVariantMap(QVariantMap &map) override;
    QVariantMap &toVariantmap(QVariantMap &map) const override;

    //// AbstractData interface
    bool isValid() const override;

};
}


#endif // DBOBJECT_H
