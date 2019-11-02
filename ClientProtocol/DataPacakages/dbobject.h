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

    //// AbstractData interface
    bool isValid() const override;

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
     * @return true if structure of table compatible with this object
     */
    bool setTableStruct(const DbTableBase &tableStruct);

    /**
     * @brief getMap
     * @return map of object
     */
    QVariantMap getMap() const;

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

    DbTableBase _tableStruct;
    int _id = -1;

    //// StreamBase interface
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    /**
     * @brief fromVariantMap
     * @param map
     * @return
     */
    virtual QVariantMap& fromVariantMap(QVariantMap& map);

    /**
     * @brief toVariantMap
     * @param map
     * @return
     */
    virtual QVariantMap& toVariantMap(QVariantMap& map) const;

    /**
     * @brief fromQuery
     * @param query
     * @return
     */
    virtual bool exec(QSqlQuery *query) const;

};
}


#endif // DBOBJECT_H
