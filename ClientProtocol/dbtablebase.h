#ifndef DBTABLEBASE_H
#define DBTABLEBASE_H

#include <QSet>
#include <QString>
#include <QVariantMap>

#include "clientprotocol_global.h"

namespace ClientProtocol {

/**
 * @brief The DbTableBase class
 */
struct CLIENTPROTOCOLSHARED_EXPORT DbTableBase
{
    QString name;
    QHash<QString, QVariant::Type> keys;

    /**
     * @brief lastIdQuery
     * @return
     */
    QString lastIdQuery() const;

    /**
     * @brief isInited
     * @return
     */
    bool isInited() const;

    /**
     * @brief isValid
     * @return
     */
    bool isValid() const;

};
}



#endif // DBTABLEBASE_H
