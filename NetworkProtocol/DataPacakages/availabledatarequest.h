#ifndef AVAILABLEDATAREQUEST_H
#define AVAILABLEDATAREQUEST_H

#include "availabledata.h"

#include <accesstoken.h>


namespace NetworkProtocol {

class AvailableDataRequest :public AvailableData {
public:
    AvailableDataRequest();
    AvailableDataRequest(const Package &pkg);

    void setData(const QHash<QString, QList<int> > &data) = delete;
    AccessToken token() const;
    void setToken(const AccessToken &token);

    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

private:

    AccessToken _token;


};

}

#endif // AVAILABLEDATAREQUEST_H
