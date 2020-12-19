/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef IOBJECTPROVIDER_H
#define IOBJECTPROVIDER_H
#include "heart_global.h"

#include <QSharedPointer>
#include <dbobject.h>
#include <quasarapp.h>

namespace QH {
namespace PKG {
class AbstractData;
}

/**
 * @brief The iObjectProvider class is base interface for work with database objects.
 * Using on database writers and database caches.
 */
class HEARTSHARED_EXPORT iObjectProvider
{
public:

    /**
     * @brief getObject this method return a strong pointer to DBObject created by select method of the template object (templateVal).
     * @param templateVal This is template object with a select data base request.
     * @note This method return a database object with a type as a type of templateVal object.
     * If you want to get a object of C class but the datatabase contains object of D class then you get object with the C class and this object will be saved to cache. So next time if you want get a object D class you get a object with C class.
     * This is possible only when the objects have the same id. that is, classes C and D are no different except for a command. So this behavior should not lead to errors.
     * @return return strong pointer to DBObject ot nullptr id object not exits.
     */
    template<class TYPE>
    QSharedPointer<TYPE> getObject(const TYPE &templateVal) {

        auto val = getObjectRaw(templateVal);
        const TYPE* result = dynamic_cast<const TYPE*>(val);
        if (!result && val) {
            QuasarAppUtils::Params::log("getObject method returned object with deffirent type of TYPE,"
                                        " check getAllObjects merhod",
                                        QuasarAppUtils::Error);

        }

        return result;
    }

    /**
     * @brief getObjectRaw This method return object without test object type
     * @note if you want get object with check object type use getObject method.
     * @param templateVal This is  template object with request to database
     * @return The database object pointer (not casted)
     */
    QSharedPointer<PKG::DBObject> getObjectRaw(const PKG::DBObject &templateVal);

    /**
     * @brief getAllObjects This method execute a select method of the templateObject and return list of all selected from databaes or cache objects
     * @param templateObject This is template object for prepe a select request.
     * @param result This is return value, list of selected objects.
     * @return true if objects have in db else false.
     */
    virtual bool getAllObjects(const PKG::DBObject &templateObject,
                               QList<QSharedPointer<PKG::DBObject>> &result) = 0;

    /**
     * @brief updateObject This method execute a update method of the saveObject and save all changes into database.
     * @note This method update object in the database only. If you try update not exists object then this method return false.
     * @param saveObject This is object for updating.
     * @param wait This arguments force current thread wait for the function finishing.
     * @return true if objects is updated successful else false.
     */
    virtual bool updateObject(const QSharedPointer<PKG::DBObject>& saveObject, bool wait) = 0;

    /**
     * @brief insertObject This method execute a insert method of the saveObject and insert it into database.
     * @note This method insert object into database only. IF object is exits in the database then this method return false.
     * @param saveObject This is object for inserting.
     * @param wait This arguments force current thread wait for the function finishing.
     * @return true if objects is saved successful else false.
     */
    virtual bool insertObject(const QSharedPointer<PKG::DBObject>& saveObject, bool wait) = 0;

    /**
     * @brief deleteObject This method execute a delete method of obj and remove current object from database.
     * @param obj This is object for removing.
     * @param wait This arguments force current thread wait for the function finishing.
     * @return true if object is removed successful else false.
     */
    virtual bool deleteObject(const QSharedPointer<PKG::DBObject>& obj, bool wait) = 0;

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
