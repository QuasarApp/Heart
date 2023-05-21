
//#
//# Copyright (C) 2022-2023 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef BIGDATAWRAPER_H
#define BIGDATAWRAPER_H
#include "abstractdata.h"

namespace QH {
namespace PKG {

/**
 * @brief The BigDataWraper class is wrapper of any big data commands.
 */
class BigDataWraper: public AbstractData
{
    QH_PACKAGE_AUTO("BigDataWraper")

public:
    BigDataWraper() = default;

    /**
     * @brief data This method sets data that shold be sent as big data.
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
#endif // BIGDATAWRAPER_H
