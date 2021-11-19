/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef CUSTOMDBREQUEST_H
#define CUSTOMDBREQUEST_H
#include <QSqlQuery>
#include <QString>
#include <dbobject.h>

namespace QH {
namespace PKG {

template <class BASE>

/**
 * @brief The CustomDBRequest class intended for send to database custom select request for working with multiple tables.
 * @note This s template class, then the BASE class should be supprt results data of select request.
 *
 * Example:
 * @code{cpp}
 *
 * QH::PKG::CustomDBRequest<User> request("SELECT * FROM Users");
   QList<QSharedPointer<QH::PKG::DBObject>> result;
   if (!db->getAllObjects(request, result))
       return false;
 * @endcode
 */
class CustomDBRequest: public BASE
{
public:
    /**
     * @brief CustomDBRequest
     * @param selectRequest This is request for database.
     */
    CustomDBRequest(const QString& selectRequest) {
        _request = selectRequest;
    }

    bool isCached() const override {
        return false;
    }

    bool isValid() const override {
        return _request.contains("select", Qt::CaseInsensitive);
    }

    PrepareResult prepareSelectQuery(QSqlQuery &q) const override {
        if (!q.prepare(_request)) {
            return PrepareResult::Fail;
        }

        return PrepareResult::Success;
    }

private:
    QString _request;
};

}

}

#endif // CUSTOMDBREQUEST_H
