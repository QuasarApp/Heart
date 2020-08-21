/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef CONFIG_H
#define CONFIG_H
#include <QStandardPaths>


// network settings
#define LOCAL_SERVER    "127.0.0.1"

#define DEFAULT_PORT          3090
#define WAIT_CONFIRM_TIME     30000 // 30000 msec = 30 sec

// Data Base settings
#define DEFAULT_DB_NAME "Storage.sqlite"
#define DEFAULT_DB_PATH QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
#define DEFAULT_DB_INIT_FILE_PATH ":/sql/BaseSpace/Res/BaseDB.sql"
#define DEFAULT_UPDATE_INTERVAL 3600000 // 1 hour

// Transport Protockol settings
#define ROUTE_COUNT_LIMIT 1000
#define TRANSPORT_PACKAGES_CACHE 1000

// Other settings
#ifdef RELEASE_BUILD
    #define WAIT_TIME 30000
    #define WAIT_RESPOCE_TIME 1000
#else
    #define WAIT_TIME 300000000
    #define WAIT_RESPOCE_TIME 100000000
#endif

#endif // CONFIG_H
