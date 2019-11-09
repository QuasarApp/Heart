#ifndef BADREQUEST_H
#define BADREQUEST_H

#include "abstractdata.h"

namespace NetworkProtocol{

/**
 * @brief The BadRequest class
 */
class BadRequest : public AbstractData
{
public:
    explicit BadRequest(const QString & err = "");
    explicit BadRequest(const Package& package);

    QString err() const;
    void setErr(const QString &err);

    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

private:
    QString _err;

};

}

#endif // BADREQUEST_H
