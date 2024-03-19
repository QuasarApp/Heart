/*
 * Copyright (C) 2024-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef STREAMMULTIVERSION_H
#define STREAMMULTIVERSION_H

#include "streambase.h"

namespace QH {

/**
 * @brief The StreamMultiversion class this parser works with simple multiversion packages.
 */
class HEARTSHARED_EXPORT StreamMultiversion: public StreamBase
{
public:

    StreamMultiversion();

    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

    /**
     * @brief version override this method to sets version of package.
     * @return
     */
    virtual int version() const = 0;

    /**
     * @brief realVersion This method return value of the version that was be saved in the bytes array.
     * @return
     * @note use this method to check version of read package in the fromStream method..
     */
    virtual int realVersion() const;

private:
    int _realVersion = 0;
};
}
#endif // STREAMMULTIVERSION_H
