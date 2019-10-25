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
    QHash<QString, QVariant::Type> keys() const override;
    void setKeys(const QHash<QString, QVariant::Type> &keys) override;
    QString lastIdQuery() const override;

    ~DbTableBase() override;

    /**
     * @brief setTableMap
     * @param tableMap
     */
    void setTableMap(const QHash<QString, QVariant::Type> &);


protected:

    QString _name;

private:


    QHash<QString, QVariant::Type> _keys;
};
}



#endif // DBTABLEBASE_H
