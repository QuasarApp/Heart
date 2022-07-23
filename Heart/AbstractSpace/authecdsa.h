//#
//# Copyright (C) 2021-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef AUTHECDSA_H
#define AUTHECDSA_H
#include "heart_global.h"

#ifdef USE_HEART_SSL


#include "abstractdata.h"
#include "hcrypto/ecdsassl11.h"

#include <asynckeysauth.h>


namespace QH {

/**
 * @brief The AuthECDSA class is ecdsa implementation of the Async authentication. This implementation based on Openssl library.
 */
typedef AsyncKeysAuth<ECDSASSL11> AuthECDSA;

}
#endif

#endif // AUTHECDSA_H
