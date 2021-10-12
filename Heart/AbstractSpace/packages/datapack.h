/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef DATAPACK_H
#define DATAPACK_H

#include "abstractdata.h"


namespace QH {
namespace PKG {

/**
 * @brief DataPack this is conteiner is wraqper of the QList for transport arrays to another node.
 * @tparam Package This is type of trasported pacakges item.
 */
template<class Package>
class DataPack final: public AbstractData
{
    QH_PACKAGE(DataPack<Package>, Package::commandText() + "Pack")

public:

        DataPack(const QList<QSharedPointer<Package>> &newPackData = {}) {
        setPackData(newPackData);
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

    bool isValid() const override {
        return AbstractData::isValid() && _packData.size();
    };

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override {
        AbstractData::fromStream(stream);

        int size = 0;
        stream >> size;

        for (int i = 0; i < size; ++i) {
            auto data = QSharedPointer<Package>::create();
            stream >> *data;

            _packData.push_back(data);
        }

        return stream;
    };

    QDataStream &toStream(QDataStream &stream) const override {
        AbstractData::toStream(stream);
        stream << static_cast<int>(_packData.size());

        for (const auto &ptr: qAsConst(_packData)) {
            stream << *ptr;
        }

        return stream;
    }


private:
    QList<QSharedPointer<Package>> _packData;

};

}
}
#endif // DATAPACK_H
