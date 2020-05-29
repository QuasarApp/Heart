
#include <basenodeinfo.h>
#include "permisioncache.h"
namespace NP {

PermisionCache::PermisionCache()
{

}

bool PermisionCache::checkPermision(const BaseNodeInfo &requestNode,
                                    const QWeakPointer<DBObject> &saveObject) const {


    auto ref = saveObject.toStrongRef();

    if (!ref.isNull())
        return false;

    PermisionData request;
    request._userId = requestNode.

    return requestNode.isValid() &&
}

void PermisionCache::deleteFromCache(const WP<AbstractData> &delObj) {
    SqlDBCache::deleteFromCache(delObj);

    auto value = delObj.toStrongRef().dynamicCast<UserPermision>();

    if (!value.isNull()) {
        auto data = value.data()->getData();
        for (auto i = data.begin(); i != data.end(); ++i) {
            _permisions.remove(i.key());
        }
    }
}

void PermisionCache::saveToCache(const QWeakPointer<AbstractData> &obj) {
    SqlDBCache::saveToCache(obj);

    auto value = obj.toStrongRef().dynamicCast<UserPermision>();

    if (!value.isNull()) {
        auto data = value.data()->getData();
        for (auto i = data.begin(); i != data.end(); ++i) {
            _permisions.insert(i.key(), i.value());
        }
    }
}
}
