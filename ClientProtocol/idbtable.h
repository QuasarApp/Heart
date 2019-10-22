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
     * @brief toStringQuery
     * @return
     */
    virtual QString toStringQuery() const = 0;

    /**
     * @brief keys
     * @return
     */
    virtual QHash<QString, QVariant::Type> keys() const = 0;
    virtual ~IDbTable() = 0;

};
}
#endif // IDBTABLE_H
