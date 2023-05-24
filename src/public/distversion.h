/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DISTVERSION_H
#define DISTVERSION_H

#include <QHash>
#include <streambase.h>


namespace QH {

/**
 * @brief The DistVersion class This is information of supported versions of the destinations api.
 */
class HEARTSHARED_EXPORT DistVersion: public StreamBase {

public:

    unsigned short min() const;
    void setMin(unsigned short newMin);
    unsigned short max() const;
    void setMax(unsigned short newMax);

    /**
     * @brief getMaxСompatible return maximum available on booth nodes version.
     * @param distVersion this is dis version.
     * @return return maximum version. if this version is not found return "-1"
     */
    int getMaxCompatible(const DistVersion& distVersion) const;

    /**
     * @brief getMinСompatible return maximum available on booth nodes version.
     * @param distVersion this is dis version.
     * @return return minimum version. if this version is not found return "-1"
     */
    int getMinCompatible(const DistVersion& distVersion) const;

protected:

    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:

    /// This is minimum supported version.
    unsigned short _min = 0;

    /// This is maximum supported version.
    unsigned short _max = 0;

};


/**
 * @brief VersionData This is array of all available apis and supported its versions.
 */
typedef QHash<QString, DistVersion> VersionData;

/**
 * @brief PackagesVersionData This is some as VersionData but for int commands.
 */
typedef QHash<unsigned short, DistVersion> PackagesVersionData;

}
#endif // DISTVERSION_H
