#ifndef PAKCAGEMANAGER_H
#define PAKCAGEMANAGER_H

#include <QMutex>
#include <QSharedDataPointer>
#include <baseid.h>


namespace NP {

/**
 * @brief The PakcageManager class - contains all processed packages
 */
class PackageManager
{
public:
    PackageManager();
    ~PackageManager();

    /**
     * @brief getPkgFromArhive - return pointer tot package from arhive.
     * @param id - id of the requariment package.
     * @result pointer of the processed package, if package not return nullpt
     */
    const Package * getPkgFromArhive(const BaseId& id) const;

    /**
     * @brief contains - check packge by id
     * @param id
     * @return true if pakcge has been parsed
     */
    bool contains(const BaseId& id) const;

    /**
     * @brief processed - add package to arhice
     * @param pkg - object of package
     * @param processResult - result of method parsePackage.
     */
    void processed(const Package& pkg, char processResult);
private:
    /**
     * @brief The PackaData struct - private data of packages
     */
    struct PackaData {
        char _parseResult;
        Package *_data = nullptr;

        ~PackaData();
    };

    QMultiMap<int, BaseId> _processTime;
    QHash<BaseId, const PackaData*> _parseResults;

    mutable QMutex _processMutex;
};

}
#endif // PAKCAGEMANAGER_H
