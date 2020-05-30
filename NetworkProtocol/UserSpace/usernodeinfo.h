#ifndef USERNODEINFO_H
#define USERNODEINFO_H

#include <basenodeinfo.h>


namespace NP {

/**
 * @brief The UserNodeInfo class
 */
class NETWORKPROTOCOLSHARED_EXPORT UserNodeInfo: public BaseNodeInfo
{
public:
    explicit UserNodeInfo(QAbstractSocket * tcp = nullptr);
    bool isValid() const override;

    unsigned int userId() const;
    void setUserId(unsigned int userId);

private:
    unsigned int _userId;
};

}

#endif // USERNODEINFO_H
