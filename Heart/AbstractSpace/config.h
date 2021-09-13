/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef HEART_CONFIG_H
#define HEART_CONFIG_H
#include <QStandardPaths>

#ifdef RELEASE_BUILD
    #define WAIT_TIME 30000
    #define WAIT_RESPOCE_TIME 10000
#else
    #define WAIT_TIME 300000000
    #define WAIT_RESPOCE_TIME 100000000
#endif

// network settings
#define LOCAL_SERVER          "127.0.0.1"
#define DEFAULT_PORT          3090  // Default work port
#define WAIT_CONFIRM_TIME   WAIT_TIME // timeout for waiting responce of server or client. 30000 msec = 30 sec

// Data Base settings
#define DEFAULT_DB_NAME "Storage.sqlite" // default database name of server
#define DEFAULT_DB_PATH QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) // default location of database. in linux systems it is ~/.local/shared/<Company>/<AppName>
#define DEFAULT_DB_INIT_FILE_PATH ":/sql/DataBaseSpace/Res/BaseDB.sql" // default database file path
#define DEFAULT_UPDATE_INTERVAL 3600000 // This is interval of update database cache by default it is 1 hour

// Transport Protockol settings
#define ROUTE_CACHE_LIMIT 1000          // This is defaut count of routes in the router class obecjt.
#define TRANSPORT_PACKAGES_CACHE 1000   // This is defaut count of processed packages in the router class obecjt.

// Node Settings
#define PACKAGE_CACHE_SIZE 1000         // this is default count limit of received packages


// Other settings


#endif // HEART_CONFIG_H
