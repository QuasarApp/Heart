/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ABSTRACTHEADER_H
#define ABSTRACTHEADER_H
#include "heart_global.h"


namespace QH {

/**
 * @brief The Header struct 32 bytes.
 */
#pragma pack(push, 1)
struct HEARTSHARED_EXPORT Header {
    /**
     * @brief command of package for more information see the AbstractData::toPackage method.
     */
    unsigned short command = 0;                     //2 bytes

    /**
     * @brief headerVersion This is version of the header struct
     */
    const unsigned char headerVersion = 1;          //3 bytes

    /**
     * @brief size This is size of package data (exclude header size).
     */
    unsigned int size = 0;                          //7 bytes

    /**
     * @brief hash This is unique id of a package. id calc with CRC32 function for Qt implementation.
     */
    unsigned int hash = 0;                          //11 bytes

    /**
     * @brief triggerHash This is hash of request package that package has been responded.
     *  The server should write to which command it responds.
     */
    unsigned int triggerHash = 0;                   //15 bytes

    /**
     * @brief unusedSpace This is unused space for changes of the header struct in the future.
     */
    unsigned long long unusedSpace1 = 0;            //23 bytes
    unsigned long long unusedSpace2 = 0;            //31 bytes
    unsigned char      unusedSpace3 = 0;            //32 bytes

    /**
     * @brief Header default constructor
     */
    Header();
    ~Header() = default;

    /**
     * @brief isValid This method check header size and compare commands.
     * @return true if header is valid.
     */
    bool isValid() const;

    /**
     * @brief reset This method reset all data and set for header invalid status.
     */
    void reset();

    /**
     * @brief toString This method return string of header of package.
     * @return string value.
     */
    QString toString() const;

};
#pragma pack(pop)
}



#endif // ABSTRACTHEADER_H
