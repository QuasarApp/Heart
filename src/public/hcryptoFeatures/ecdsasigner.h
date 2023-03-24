/*
 * Copyright (C) 2021-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ECDSASIGNER_H
#define ECDSASIGNER_H

#include "signer.h"
#include "hcrypto/ecdsassl11.h"

namespace QH {

/**
 * @brief The ECDSASigner class is ecdsa implementation of the Signer class. This implementation based on Openssl library.
 */
typedef Signer<ECDSASSL11> ECDSASigner;

}
#endif // ECDSASIGNER_H
