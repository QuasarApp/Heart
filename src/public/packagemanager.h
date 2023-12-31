/*
 * Copyright (C) 2018-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef PAKCAGEMANAGER_H
#define PAKCAGEMANAGER_H

#include "package.h"

#include <QMutex>
#include <QSharedDataPointer>


namespace QH {

class PackaData;

/**
 * @brief The PakcageManager class contains all processed packages. Working like a cache of packages.
 */
class HEARTSHARED_EXPORT PackageManager
{
public:
    PackageManager();
    ~PackageManager();

    /**
     * @brief getPkgFromArhive This method return pointer to package from archive.
     * @param id This is id of the requirement package.
     * @result pointer of the processed package, if package not find then return nullptr.
     */
    const Package * getPkgFromArhive(const unsigned int &id) const;

    /**
     * @brief contains This method checks if the package contains in this container by id.
     * @param id This is unique key of package.
     * In Default implementation it is qHash of package.
     * @return True if the pakcage has been parsed.
     */
    bool contains(const unsigned int& id) const;

    /**
     * @brief processed This method add a package to the archive.
     * @param pkg This is object of the package.
     * @param processResult This is result of method parsePackage.
     * For more information see ParserResult enum.
     */
    void processed(const Package& pkg, char processResult);
private:

    QMultiMap<int, unsigned int> _processTime;
    QHash<unsigned int, const PackaData*> _parseResults;

    mutable QMutex _processMutex;
};

}
#endif // PAKCAGEMANAGER_H
