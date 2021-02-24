/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef SINGLESERVERBASE_H
#define SINGLESERVERBASE_H

#include "databasenode.h"


namespace QH {

/**
 * @brief The SingleServerBase class This is base class of SingleClient adn SingleServer
 */
class SingleBase: public DataBaseNode
{
    Q_OBJECT
public:
    SingleBase();

protected:
    /**
     * @brief checkToken This method check pkg on token validation sopport.
     * @param pkg This is parsed package.
     * @return true if the package not support token validation.
     *
     * For get more information see  IToken class.
     */
    bool checkToken(const PKG::AbstractData *pkg) const;
};
}
#endif // SINGLESERVERBASE_H
