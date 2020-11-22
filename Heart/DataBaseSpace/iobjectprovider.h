/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef IOBJECTPROVIDER_H
#define IOBJECTPROVIDER_H
#include "heart_global.h"
#include "promise.h"

#include <QSharedPointer>
#include <dbobject.h>
#include <quasarapp.h>
#include <future>

namespace QH {
namespace PKG {
class AbstractData;
}

/**
 * @brief The iObjectProvider class is base interface for work with database objects.
 * Using on database writers and database cache.
 */
class HEARTSHARED_EXPORT iObjectProvider
{
public:
    iObjectProvider();
    virtual ~iObjectProvider();

    /**
     * @brief getObjectRaw This method return object without test object type
     * @note if you want get object with check object type use getObject method.
     * @param templateVal This is  template object with request to database
     * @return The database object pointer (not casted)
     */
    Promise<const PKG::DBObject *> getObject(const PKG::DBObject &templateVal);

    /**
     * @brief getAllObjects This method executable select method of objects and return list of all selected objects
     * @param templateObject This is template object with select request.
     * @param result This is return value, pointer to promise of the list of selected objects.
     * @return true if the selectQuery method of the Database invoked successful.
     */
    virtual bool getAllObjects(const PKG::DBObject &templateObject,
                               Promise<QList<const PKG::DBObject *>> &result) = 0;

    /**
     * @brief updateObject This method executable update method of objects and save the change of current object into database.
     * @param saveObject This is object for updating.
     * @return true if objects is updated successful else false.
     */
    virtual bool updateObject(const PKG::DBObject* saveObject) = 0;

    /**
     * @brief insertObject This method executable insert method of objects and save current object into database.
     * @param saveObject This is object for inserting.
     * @return true if objects is saved successful else false.
     */
    virtual bool insertObject(const PKG::DBObject* saveObject) = 0;

    /**
     * @brief deleteObject This method executable delete method of objects and remove current object from database.
     * @param obj This is object for removing.
     * @return true if object is removed successful else false.
     */
    virtual bool deleteObject(const PKG::DBObject* obj) = 0;

    /**
     * @brief setSQLSources This method set sql sources for deployed database.
     * @param list This is list of sql files with database sources.
     *
     * For more information about sql sources see the DataBaseNode::SQLSources method.
     */
    virtual void setSQLSources(const QStringList& list) = 0;

};

}
#endif // IOBJECTPROVIDER_H
