/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef SOFTDELETE_H
#define SOFTDELETE_H

namespace QH {

/**
 * @brief The SoftDelete class povide the soft delete functionality.
 * All child classes of this class must be deleted using the softDelete method for preparring for delete.
 * IF you delete object without prepare then distructor invoke the abort function in debug mode.
 * For release mode distructor print error message only.
 */
class SoftDelete {
public:
    SoftDelete();
    virtual ~SoftDelete();

    /**
     * @brief softDelete This method remove this object and save all changes into database.
     * @note Use only this method for delete all objects of child classes of this class.
     */
    void softDelete();

protected:
    /**
     * @brief prepareForDelete This method must be prepare this object for delete.
     * Override this for working main functions of this class.
     */
    virtual void prepareForDelete() = 0;

private:
    bool fSoftDelete = false;
};
}
#endif // SOFTDELETE_H
