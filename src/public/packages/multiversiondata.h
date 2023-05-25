/*
 * Copyright (C) 2023-2023 QuasarApp.
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
    std::function<QDataStream& (QDataStream& stream)> from;
    std::function<QDataStream& (QDataStream& stream)> to;
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
 *          MyPackage(): MultiversionData(
 *              {0, {[](auto stream){
 *                  // some code from stream for version 0 ;
 *                  return stream;
 *              },{[](auto stream){
 *                  // some code to stream for version 0 ;
 *                  return stream;
 *              }},
 *
 *              1, {[](auto stream){
 *                  // some code from stream for version 1 ;
 *                  return stream;
 *              },{[](auto stream){
 *                  // some code to stream for version 1 ;
 *                  return stream;
 *              }},}
 *          ){
 *          }
 *      }
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

    QDataStream& fromStream(QDataStream& stream) override final;
    QDataStream& toStream(QDataStream& stream) const override final;
    QDataStream& toStreamOf(QDataStream& stream, unsigned short version) const override final;

private:

    QMap<unsigned short /*version*/, SerializationBox> _serializers;
};
}
}
#endif // MULTIVERSIONDATA_H
