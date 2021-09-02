/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef HEART_GLOBAL_H
#define HEART_GLOBAL_H

#include <QtCore/qglobal.h>

#define HEART_BUILD_LVL 1

#if defined(HEART_LIBRARY)
#  define HEARTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define HEARTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // HEART_GLOBAL_H
