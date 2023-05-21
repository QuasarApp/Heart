/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DATAPACK_H
#define DATAPACK_H

#include <abstractdata.h>

namespace QH {
namespace PKG {

/**
 * @brief DataPack this is conteiner is wraqper of the QList for transport arrays to another node.
 * @tparam Package This is type of trasported pacakges item.
 * @note All packs data objects should be inherited of the UniversalData class.
 *  This is due to classes base on abstract data may broken transporting data between nodes if you change toStream and fromStream methods.
 *  To fix this issue, we use QVariantMap container for parsing data.
 * @see UniversalData
 */
template<class Package>
class DataPack final: public AbstractData
{
    QH_PACKAGE(Package::commandText() + "Pack", Package::version())

public:

        DataPack(const QList<QSharedPointer<Package>> &newPackData = {}) {
        setPackData(newPackData);
#ifdef HEART_VALIDATE_PACKS
            static_assert(std::is_base_of_v<UniversalData, Package> &&
                      "The template class of DataPack must be child of the UniversalData class");
#endif
    }

    /**
     * @brief size This method return of the items count of this pack.
     * @return size of the packs.
     */
    unsigned int size() const {
        return _packData.size();
    }

    /**
     * @brief packData This method return source list of the elements.
     * @return source list
     */
    const QList<QSharedPointer<Package>> &packData() const {
        return _packData;
    }

    /**
     * @brief setPackData This method sets new lsit of pacakges.
     * @param newPackData This is new source lsit.
     */
    void setPackData(const QList<QSharedPointer<Package>> &newPackData) {
        _packData = newPackData;
    }

    /**
     * @brief push This method append @a data to end of list.
     * @param data This is new data pacakge that will be added into back of this list.
     */
    void push(const QSharedPointer<Package>& data) {
        _packData.push_back(data);
    };

    /**
     * @brief push This method append @a data to end of list.
     * @param data This is new data pacakge that will be added into back of this list.
     */
    void push(const Package& data) {
        _packData.push_back(QSharedPointer<Package>::create(data));
    };

    /**
     * @brief isValid This implementation check all items of the pack to valid and packa size. The pack size should be more then 0.
     * @return true if the pack of items is valid else flase..
     */
    bool isValid() const override {

        if (!_packData.size()) {
            return false;
        }

        for (const auto& it: _packData) {
            if (!it->isValid()) {
                return false;
            }
        }

        return AbstractData::isValid();
    };

    /**
     * @brief token is custom data block.
     * @note The custom data block do not validate.
     *  If you want to add validation for custom data block then override the isValid method.
     * @return custom data block
     */
    const QByteArray &customData() const {
        return _data;
    }

    /**
     * @brief setCustomData This method sets custom data block
     * @param newToken
     */
    void setCustomData(const QByteArray &newToken) {
        _data = newToken;
    }

protected:
    QDataStream &fromStream(QDataStream &stream) override {

        int size = 0;
        stream >> size;

        for (int i = 0; i < size; ++i) {
            auto data = QSharedPointer<Package>::create();
            stream >> *data;

            _packData.push_back(data);
        }

        stream >> _data;

        return stream;
    };

    QDataStream &toStream(QDataStream &stream) const override {
        stream << static_cast<int>(_packData.size());

        for (const auto &data: qAsConst(_packData)) {
            stream << *data;
        }

        stream << _data;

        return stream;
    }

    QDataStream &toStreamOf(QDataStream &stream, unsigned short version) const override {
        stream << static_cast<int>(_packData.size());

        for (const auto &data: qAsConst(_packData)) {
            data->toStreamOf(stream, version);
        }

        stream << _data;

        return stream;
    }

private:
    QList<QSharedPointer<Package>> _packData;
    QByteArray _data;

};

}
}
#endif // DATAPACK_H
