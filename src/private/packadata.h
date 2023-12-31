/*
 * Copyright (C) 2018-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef PACKADATA_H
#define PACKADATA_H

namespace QH {
class Package;
/**
 * @brief The PackaData struct - private data of packages
 */
struct PackaData {
    char _parseResult;
    Package *_data = nullptr;

    ~PackaData();
};
}
#endif // PACKADATA_H
