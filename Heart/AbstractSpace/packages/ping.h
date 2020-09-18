#ifndef PING_H
#define PING_H

#include "abstractdata.h"


namespace QH {
namespace PKG {

/**
 * @brief The Ping class - test class for translate data on network
 */
class HEARTSHARED_EXPORT Ping: public AbstractData
{
public:
    Ping();
    Ping(const Package& from);

    // AbstractData interface
    /**
     * @brief isValid - heck this package if valid.
     * @return
     */
    bool isValid() const;

    /**
     * @brief ansver This is bool value for check this objec that it si ansver or request.
     *  using on tessts.
     * @return true if this object is ansver from server
     */
    bool ansver() const;

    /**
     * @brief setAnsver - set the ansver otion
     * @param ansver - new ansver value
     */
    void setAnsver(bool ansver);

protected:
    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

private:
    bool _ansver = false;
};
}
}
#endif // PING_H



