/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SOFTDELETE_H
#define SOFTDELETE_H
#include "heart_global.h"

namespace QH {

/**
 * @brief The SoftDelete class povide the soft delete functionality.
 * All child classes of this class must be deleted using the softDelete method for preparring for delete.
 * IF you delete object without prepare then destructor invoke the abort function in debug mode.
 * For release mode destructor print error message only.
 */
class HEARTSHARED_EXPORT SoftDelete {
public:
    SoftDelete();
    /**
     * @brief ~SoftDelete This destructor has bin crashed if the softDelete method not will be invoked before delete this object.
     * IF you try delete this object on release distribution then you get error message only without invoked std::abort method.
     * @warning do not delete child classes using the delete operator or deleteLater method of the QObject class.
     */
    virtual ~SoftDelete();

    /**
     * @brief softDelete This method remove this object and save all changes into database.
     * @note Use only this method for delete all objects of child classes of this class.
     */
    void softDelete();

protected:
    /**
     * @brief prepareForDelete This method must be prepare object for delete.
     * Override this for working main functions of this class.
     * For more information see the SoftDelete class and SoftDelete::~SoftDelete distructor.
     */
    virtual void prepareForDelete() = 0;

private:
    bool fSoftDelete = false;
};
}
#endif // SOFTDELETE_H
