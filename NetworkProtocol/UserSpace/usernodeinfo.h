#ifndef USERNODEINFO_H
#define USERNODEINFO_H

#include <basenodeinfo.h>


namespace NP {

/**
 * @brief The UserNodeInfo class
 */

class UserBaseData;

class NETWORKPROTOCOLSHARED_EXPORT UserNodeInfo: public BaseNodeInfo
{
public:
    explicit UserNodeInfo(QAbstractSocket * tcp = nullptr);
    bool isValid() const override;

    /**
     * @brief destUserId
     * @return id of
     */
    unsigned int destUserId() const;
    void setDestUserId(unsigned int destUserId);

    UserBaseData *getCurrentUserData() const;
    void setCurrentUserData(UserBaseData *value);

private:
    /**
     * @brief _destUserId - destanation node userid
     */
    unsigned int _destUserId;

    /**
     * @brief currentUserData - current node user data.
     */
    UserBaseData * currentUserData = nullptr;

};

}

#endif // USERNODEINFO_H
