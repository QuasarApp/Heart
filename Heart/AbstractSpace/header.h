/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTHEADER_H
#define ABSTRACTHEADER_H
#include "heart_global.h"


namespace QH {
/**
 * @brief The Header struct 12 bytes.
 */
#pragma pack(push, 1)
struct HEARTSHARED_EXPORT Header{
    /**
     * @brief size This is size of package data (exclude header size)
     */
    unsigned short size;

    /**
     * @brief command of pacage for more information see the AbstractData::toPackage method.
     */
    unsigned short command;

    /**
     * @brief hash This is uniqueue id of a package. id calc with CRC32 function fo Qt implamentation. qHash(QByteArray)
     */
    unsigned int hash;

    /**
     * @brief triggerHash This is hash of request package that this package has been responded
     *  the server should write to which command it responds.
     */
    unsigned int triggerHash;

    /**
     * @brief Header default constructor
     */
    Header();
    ~Header() = default;

    /**
     * @brief isValid This method check header size and compare commands.
     * @return true if header is valid
     */
    bool isValid() const;

    /**
     * @brief reset This method reset all data and set for header invalid status
     */
    void reset();

    /**
     * @brief toString This method return string of header of package
     * @return string value.
     */
    QString toString() const;

};
#pragma pack(pop)

}



#endif // ABSTRACTHEADER_H
