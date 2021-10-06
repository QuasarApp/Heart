/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef PING_H
#define PING_H

#include "abstractdata.h"


namespace QH {
namespace PKG {

/**
 * @brief The Ping class - test class for translate data on network.
 */
class HEARTSHARED_EXPORT Ping: public AbstractData
{
    QH_PACKAGE(Ping, "Ping")

public:
    Ping();
    Ping(const Package& from);

    // AbstractData interface
    /**
     * @brief isValid - Check this package if valid.
     * @return True if package is valid.
     */
    bool isValid() const override;

    /**
     * @brief answer This is bool value for check this object that it is answer or request.
     *  Using on tests.
     * @return True if this object is answer from server.
     */
    bool ansver() const;

/* I thing you need to correct the name of this method. From "setAnsver" to "setAnswer" */

    /**
     * @brief setAnsver - Set the answer option.
     * @param ansver - New answer value.
     */
    void setAnsver(bool ansver);

protected:
    // StreamBase interface
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

private:
    bool _ansver = false;
};
}
}
#endif // PING_H



