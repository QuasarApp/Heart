
//#
//# Copyright (C) 2022-2025 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef BIGDATAWRAPPER_H
#define BIGDATAWRAPPER_H
#include "heart_global.h"
#include "abstractdata.h"

namespace QH {
namespace PKG {

/**
 * @brief The BigDataWrapper class is wrapper of any big data commands.
 */
class HEARTSHARED_EXPORT BigDataWrapper: public AbstractData
{
    QH_PACKAGE("BigDataWrapper")

public:
    BigDataWrapper() = default;

    /**
     * @brief data This method sets data that should be sent as big data.
     * @return
     */
    const AbstractData *data() const;

    /**
     * @brief setData This method sets new data for this wrapper.
     * @param newData this is new data object.
     */
    void setData(const AbstractData *newData);
    QString toString() const override;
    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    const AbstractData *_data = nullptr;



};
}
}
#endif // BIGDATAWRAPPER_H
