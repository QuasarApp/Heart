#ifndef PERMISIONDATA_H
#define PERMISIONDATA_H

#include <QString>
#include <abstractkey.h>
#include "dbaddress.h"

namespace NP {

/**
 * @brief The PermisionData class- this class provide unique key for permison of subject (id) to object (address).
 */
class NETWORKPROTOCOLSHARED_EXPORT PermisionData: public AbstractKey, public StreamBase {

public:

    PermisionData() = default;
    PermisionData(const BaseId& subject, const DbAddress& objcet);

    friend bool operator == (const PermisionData& left, const PermisionData& right);

    /**
     * @brief qHash - calc unique key of PermisionData
     * @param userPermision
     * @return unique key
     */
    unsigned int hash() const override;


    const BaseId & id() const override;
    const QString &table() const override;
    bool isValid() const override;
    bool equal(const AbstractKey *other) const override;

    void setId(const BaseId &Id);

    DbAddress address() const;
    void setAddress(const DbAddress &address);

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:

    /// id  of user of node
    BaseId _id;

    /// table of target object (second part of key)
    DbAddress _address;


};

}
#endif // PERMISIONDATA_H
