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
