/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef PERMISIONS_H
#define PERMISIONS_H
namespace QH {

/**
 * @brief The Permission enum
 * permision to data in database
 */
enum class Permission: unsigned char {
    /// The network member do not have any permisions for an object
    NoPermission = 0x00,
    /// The network member can read a object.
    Read = 0x01,
    /// The network member can Read and Write a new data into object. But can not be removed.
    Update = 0x02,
    /// The network member have all permisions to an object
    Write = 0x03,
};

}
#endif // PERMISIONS_H
