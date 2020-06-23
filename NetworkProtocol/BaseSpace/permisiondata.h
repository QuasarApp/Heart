#ifndef PERMISIONDATA_H
#define PERMISIONDATA_H

#include <QString>
#include <abstractkey.h>
#include "dbaddress.h"
#include "permisions.h"

namespace NP {

/**
 * @brief The PermisionData class- this class provide unique key for permison of subject (id) to object (address).
 */
class NETWORKPROTOCOLSHARED_EXPORT PermisionData: public AbstractKey {

public:

    PermisionData() = default;
    PermisionData(const DbId& subject, const DbAddress& objcet);

    friend bool operator == (const PermisionData& left, const PermisionData& right);

    /**
     * @brief qHash - calc unique key of PermisionData
     * @param userPermision
     * @return unique key
     */
    unsigned int hash() const override;


    DbId id() const;
    void setId(const DbId &Id);

    DbAddress address() const;
    void setAddress(const DbAddress &address);

private:

    /// id  of user of node
    DbId _id;

    /// table of target object (second part of key)
    DbAddress _address;
};

}
#endif // PERMISIONDATA_H
