#ifndef PING_H
#define PING_H

#include "abstractdata.h"


namespace QH {

/**
 * @brief The Ping class - test class for translate data on network
 */
class HEARTSHARED_EXPORT Ping: public AbstractData
{
public:
    Ping();
    Ping(const Package& from);

    // AbstractData interface
    bool isValid() const;
    bool copyFrom(const AbstractData *);

    bool ansver() const;
    void setAnsver(bool ansver);

protected:
    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

private:
    bool _ansver = false;
};
}
#endif // PING_H



