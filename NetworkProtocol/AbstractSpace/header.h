/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTHEADER_H
#define ABSTRACTHEADER_H
#include "networkprotocol_global.h"


namespace NP {
/**
 * @brief The Header struct 12 byte
 */
#pragma pack(push, 1)
struct NETWORKPROTOCOLSHARED_EXPORT Header{
    /**
     * @brief size - size of package data (not header)
     */
    unsigned short size;

    /**
     * @brief command of pacage
     */
    unsigned short command;

    /**
     * @brief hash - is uniqueue id of a package. id calc with CRC32 function fo Qt implamentation. qHash(QByteArray)
     */
    unsigned int hash;

    /**
     * @brief triggerHash - this is hash of request package that this package has been responded
     * the server should write to which command it responds
     */
    unsigned int triggerHash;

    /**
     * @brief Header default constructor
     */
    Header();
    ~Header() = default;

    /**
     * @brief isValid
     * @return true if header is valid
     */
    bool isValid() const;


    /**
     * @brief reset - reset all data and set for header invalid status
     */
    void reset();

    /**
     * @brief toString - return string of header of package
     * @return
     */
    QString toString() const;

};
#pragma pack(pop)

}



#endif // ABSTRACTHEADER_H
