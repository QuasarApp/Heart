#ifndef AVAILABLEDATAREQUEST_H
#define AVAILABLEDATAREQUEST_H

#include "availabledata.h"


namespace ClientProtocol {

class AvailableDataRequest :public AvailableData {
public:
    AvailableDataRequest();
    void setData(const QHash<QString, QList<int> > &data) = delete;
};

}

#endif // AVAILABLEDATAREQUEST_H
