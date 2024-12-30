/*
 * Copyright (C) 2023-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef MULTIVERSIONDATA_H
#define MULTIVERSIONDATA_H

#include "abstractdata.h"


namespace QH {
namespace PKG {

struct SerializationBox {
    std::function<QDataStream& (QDataStream& stream)> from = nullptr;
    std::function<QDataStream& (QDataStream& stream)> to = nullptr;
};

/**
 * @brief The MultiversionData class This class add support for multiple versions of ne command/package.
 * If you need to add support multiple versions make your pakcage class children of the MultiversionData class.
 *
 *  Example:
 *
 *  @code{cpp}
 *
 *      class MyPackage: public MultiversionData {
            MyPackage():  QH::PKG::MultiversionData(
                    {
                        {0, // version 0
                            {
                                [this](QDataStream& stream) -> QDataStream&{ // from
                                    stream >> v1;
                                    return stream;
                                },
                                [this](QDataStream& stream) -> QDataStream&{ // to
                                    stream << v1;

                                    return stream;
                                }
                            }
                        },
                        {1, // version 1
                            {
                                [this](QDataStream& stream) -> QDataStream&{ // from
                                    stream >> v1;
                                    stream >> v2;

                                    return stream;
                                },
                                [this](QDataStream& stream) -> QDataStream&{ // to
                                    stream << v1;
                                    stream << v2;
                                    return stream;
                                }
                            }
                        }
                    }
                    ) {};
 *  @endcode
 *
 *  @note the default toBytes function of this class will be convert your class using latest version.
 */
class HEARTSHARED_EXPORT MultiversionData: public AbstractData
{
public:
    /**
     * @brief MultiversionData main constructor
     * @param serializers this is map of the all supported versions.
     */
    MultiversionData(const QMap<unsigned short /*version*/, SerializationBox>& serializers);

    /**
     * @brief packageVersion This method should be return number of the pacakge version.
     * @return pcakge version. by default return - 0 (any version)
     */
    const DistVersion& packageVersion() const;

    QDataStream& fromStream(QDataStream& stream) override final;
    QDataStream& toStream(QDataStream& stream) const override final;

    /**
     * @brief toBytesOf This is overload method of StreamBase::toBytes for support multi versions of packages.
     * @param version This is required version pacakge.
     * @return bytes array for package.
     * @note This is just wrapper method for the AbstractData::toStream method.
     */
    QByteArray toBytesOf(const DistVersion &version) const;

    /**
     * @brief toStreamOf This overrload of the base toStream method for support the multi version packages.
     * @param stream this is stream object.
     * @param version this is custom version of parsing function.
     * @return stream object.
     */
    QDataStream& toStreamOf(QDataStream& stream, const DistVersion &version) const;


    bool toPackage(Package &package, const DistVersion &reqVersion, unsigned int triggerHash = 0) const override final;


private:
    DistVersion _packageVersion;
    QMap<unsigned short /*version*/, SerializationBox> _serializers;


};
}
}
#endif // MULTIVERSIONDATA_H
