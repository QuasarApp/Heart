/*
 * Copyright (C) 2018-2022 QuasarApp.
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
 * @brief The DistVersion class This is infirmation of supported versions of the distanation api.
 */
class DistVersion: public StreamBase {

public:

    unsigned short min() const;
    void setMin(unsigned short newMin);
    unsigned short max() const;
    void setMax(unsigned short newMax);

protected:

    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:

    /// This is monimum supported version.
    unsigned short _min = 0;

    /// This is maximum supported version.
    unsigned short _max = 0;

};


/**
 * @brief VersionData This is array of all avalable apis and supported its versions.
 */
typedef QHash<QString, DistVersion> VersionData;

}
#endif // DISTVERSION_H
