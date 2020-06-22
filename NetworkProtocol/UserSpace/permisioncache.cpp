
#include <usernodeinfo.h>
#include "permisioncache.h"
#include "sqldbwriter.h"
#include "quasarapp.h"

namespace NP {

PermisionCache::PermisionCache()
{

}

bool PermisionCache::checkPermision(const UserNodeInfo &requestNode,
                                    const DBObject &object,
                                    Permission requiredPermision) {


    PermisionData data;
    data._userId = requestNode.userId();
    data._objectTable = object.tableName();
    data._idObject = object.getId();

    auto permision = SP<UserPermision>::create(data);
    auto request = permision.dynamicCast<DBObject>();
    if (!getObject(request)) {
        return false;
    }

    return requestNode.isValid() && permision->isHavePermision(data, requiredPermision);
}

bool PermisionCache::saveObject(const DBObject *saveObject) {
    auto value = saveObject.toStrongRef().dynamicCast<UserPermision>();

    if (value.isNull()) {
        return SqlDBCache::saveObject(saveObject);
    }

    saveToCache(value);

    if (getMode() == SqlDBCasheWriteMode::Force) {
        if (!_writer.isNull() && _writer->isValid()) {
            if (!_writer->saveObject(saveObject)) {
                return false;
            }

            return true;
        }
    } else {
        auto &data = value->getData();
        for (auto it = data.begin(); it != data.end(); ++it) {
            _needToSave.insert(it.key());
        }

        globalUpdateDataBase(getMode());
    }

    return true;
}

void PermisionCache::deleteFromCache(const WP<AbstractData> &delObj) {

    auto value = delObj.toStrongRef().dynamicCast<UserPermision>();

    if (value.isNull()) {
        SqlDBCache::deleteFromCache(delObj);
        return;
    }

    auto data = value.data()->getData();
    for (auto i = data.begin(); i != data.end(); ++i) {
        _permisions.remove(i.key());
    }
}

void PermisionCache::saveToCache(const DBObject *obj) {

    auto value = obj.toStrongRef().dynamicCast<UserPermision>();

    if (value.isNull()) {
        SqlDBCache::saveToCache(obj);
        return;
    }

    auto data = value.data()->getData();
    for (auto i = data.begin(); i != data.end(); ++i) {
        _permisions.insert(i.key(), i.value());
    }
}

bool PermisionCache::getFromCache(DBObject *obj) {
    auto value = obj.dynamicCast<UserPermision>();

    if (value.isNull()) {
        return SqlDBCache::getFromCache(obj);
    }

    auto data = value.data()->getData();

    for (auto i = data.begin(); i != data.end(); ++i) {
        if (_permisions.contains(i.key())) {
            value->setData(i.key(), _permisions.value(i.key()));
        } else  {
            return false;
        }
    }

    return true;
}

void PermisionCache::globalUpdateDataBasePrivate(qint64 currentTime) {
    SqlDBCache::globalUpdateDataBasePrivate(currentTime);

    QMutexLocker lock(&_savePermisionLaterMutex);

    SP<UserPermision> tempSP;

    for (const auto& i : _needToSave) {

        UserPermision saveObjcet(i, _permisions.value(i));

        if (!_writer.isNull() && _writer->isValid()) {
            tempSP.reset(&saveObjcet);
            if (!_writer->saveObject(tempSP)) {
                QuasarAppUtils::Params::log("writeUpdateItemIntoDB failed when write permision"
                                            " user id:" + QString::number(i._userId) +
                                            " table :" + i._objectTable +
                                            " objectId : " + QString::number(i._idObject),
                                            QuasarAppUtils::VerboseLvl::Error);
            }

        } else {
            QuasarAppUtils::Params::log("writeUpdateItemIntoDB failed when"
                                        " db writer is npt inited! ",
                                        QuasarAppUtils::VerboseLvl::Error);
        }

    }
}
}
