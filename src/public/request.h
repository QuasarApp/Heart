/*
 * Copyright (C) 2018-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef IREQUEST_H
#define IREQUEST_H

#include "heart_global.h"

namespace QH {

/**
 * @brief The Request class is base interface for working with requests commands.
 *
 * Example ovveriding Requests:
 *\code {cpp}
    class HEARTSHARED_EXPORT MyRequest: public MyData, public Request
    {
    public:
        MyRequest();
        unsigned char getRequestCmd() const override {
            return _request;
        };

        void setRequest(const UserRequestType &request) {
            _request = request;
        }

        ...

    private:
        int _request;
    };
 *
 *\endcode
 *
 * So next you need to parse all requests data of p the MyData pacakge.
 * \code{cpp}
 * bool SingleServer::workWithMyDataRequest(const MyData* objr) {

    auto request = dynamic_cast<const Request*>(obj);

    if (request) {
        return false;
    }
    ...
    }
 * \endcode
 */
class HEARTSHARED_EXPORT Request
{
public:
    Request();
    /**
     * @brief getRequestCmd This method return request command
     * @return request command value.
     */
    virtual unsigned char getRequestCmd() const = 0;

};
}
#endif // IREQUEST_H
