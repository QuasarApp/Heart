/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef CACHEDDBOBJECTSREQUEST_H
#define CACHEDDBOBJECTSREQUEST_H

#include <QString>


namespace QH {
namespace PKG {

template <class BASE>
/**
 * @brief The CachedDbObjectsRequest class This is template class for generate request with the custom condition for BASE objects.
 * @note This request send request to the hardware and not used a cache to get database objects. But all objectes will be cahed for next singel requests.
 * @warning if BASE object is not cacheble then result objects well  not be saved into cache.
 *
 * For more information see the DBObject::isCached method.
 *
 * Example:
 * \code{cpp}
        QH::PKG::CachedDbObjectsRequest<User> request("points > 10");

        QList<const QH::PKG::DBObject *> result;
        if (!db()->getAllObjects(request, result)) {
            return false;
        }
 * \endcode
 */
class CachedDbObjectsRequest : public BASE
{
public:
    CachedDbObjectsRequest(const QString& condition) {
        _condition = condition;
    }

    QString condition() const override final{
        return _condition;
    }

    bool isCached() const override {
        return false;
    }

private:
    QString _condition;
};

}

}
#endif // CACHEDDBOBJECTSREQUEST_H
