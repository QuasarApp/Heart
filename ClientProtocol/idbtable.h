#ifndef IDBTABLE_H
#define IDBTABLE_H
#include "clientprotocol_global.h"

#include <QStringList>
#include <QVariant>

namespace ClientProtocol {

/**
 * @brief The IDbTable class
 */
class CLIENTPROTOCOLSHARED_EXPORT  IDbTable
{
public:
    /**
     * @brief IDbTable
     */
    IDbTable();

    /**
     * @brief name
     * @return
     */
    virtual QString name() const = 0;

    /**
     * @brief keys
     * @return
     */
    virtual QHash<QString, QVariant::Type> keys() const = 0;

    /**
     * @brief setKeys
     * @param keys
     */
    virtual void setKeys(const QHash<QString, QVariant::Type> &keys) = 0;

    /**
     * @brief lastId
     * @return query for get last id of table in db
     */
    virtual QString lastIdQuery() const = 0;


    virtual ~IDbTable() = 0;


};
}
#endif // IDBTABLE_H
