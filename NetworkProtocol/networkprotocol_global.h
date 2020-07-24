/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef NETWORKPROTOCOL_GLOBAL_H
#define NETWORKPROTOCOL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(NETWORKPROTOCOL_LIBRARY)
#  define NETWORKPROTOCOLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define NETWORKPROTOCOLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // NETWORKPROTOCOL_GLOBAL_H
