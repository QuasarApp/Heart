/*
 * Copyright (C) 2021-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/



#ifndef BIGDATAHEADER_H
#define BIGDATAHEADER_H

#include "bigdatabase.h"


namespace QH {
namespace PKG {

/**
 * @brief The BigDataHeader class is first empty pacakge with information of big data package.
 *
 * **Transfer algorithm**
 *
 * 1. The big data pacakge converted to bytearray.
 * 2. Prepare The BigDataHeader pacakge with unique id and count of the parts
 * 3. Part data pacakages.
 */
class HEARTSHARED_EXPORT BigDataHeader : public BigDataBase
{
    QH_PACKAGE_AUTO("BigDataHeader")

public:
    BigDataHeader();

    bool isValid() const override;
    QString toString() const override;

    // StreamBase interface
    /**
     * @brief getPackagesCount This method return count of parts of bigdata package.
     * @return count of parts of bigdata package.
     */
    int getPackagesCount() const;

    /**
     * @brief setPackagesCount This method sets new count of pcakges.
     * @param newPackagesCount This is new value of the parts count.
     */
    void setPackagesCount(int newPackagesCount);

    /**
     * @brief getCommand This method return commnad of moving package.
     * @return commnad of moving package.
     */
    unsigned short getCommand() const;

    /**
     * @brief setCommand This method sets new value for package command that will moved
     * @param newCommand new value for package command that will moved
     */
    void setCommand(unsigned short newCommand);

    /**
     * @brief pkgVersion This is version of the moving big data package.
     * @return version of the moving big data package.
     */
    unsigned short pkgVersion() const;

    /**
     * @brief setPkgVersion This method sets new version of bid data package.
     * @param newVersion This is new version
     */
    void setPkgVersion(unsigned short newVersion);

protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    int packagesCount;
    unsigned short _command = 0;
    unsigned short _version = 0;
};
}
}
#endif // BIGDATAHEADER_H
