#ifndef DBTABLEBASE_H
#define DBTABLEBASE_H

#include "idbtable.h"

#include <QSet>
#include <QString>
#include <QVariantMap>


namespace ClientProtocol {

/**
 * @brief The DbTableBase class
 */
class CLIENTPROTOCOLSHARED_EXPORT DbTableBase : public IDbTable
{
public:
    DbTableBase(QString& name);

    // IDbTable interface:
    QString name() const override;
    QString toStringQuery() const override;
    QHash<QString, QVariant::Type> keys() const override;
    QString lastIdQuery() const override;

    ~DbTableBase() override;

    /**
     * @brief tableMap
     * @return
     */
    QVariantMap tableMap() const;

    /**
     * @brief setTableMap
     * @param tableMap
     */
    void setTableMap(const QVariantMap &tableMap);

    /**
     * @brief dependencies
     * @return
     */
    QHash<QString, QString> dependencies() const;

    /**
     * @brief addDependecies
     * @param table
     * @param val
     * @return
     */
    bool addDependecies(const IDbTable* table, const QString &val);

protected:
    void setDependencies(const QHash<QString, QString> &dependencies);

    QString _name;
    QVariantMap _tableMap;
    QHash<QString, QString> _dependencies;

private:
    /**
     * @brief getType
     * @param str
     * @return
     */
    QVariant::Type getType(const QString& str);

    QHash<QString, QVariant::Type> _keys;
};
}



#endif // DBTABLEBASE_H
