/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef CONFIG_H
#define CONFIG_H
#include <QStandardPaths>

#define LOCAL_SERVER    "127.0.0.1"

#define DEFAULT_PORT          3090

#define DEFAULT_DB_NAME "NodeStorage.sqlite"
#define DEFAULT_DB_PATH QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
#define DEFAULT_DB_INIT_FILE_PATH ":/sql/BaseSpace/Res/BaseDB.sql"
#define DEFAULT_UPDATE_INTERVAL 3600000 // 1 hour

#define ROUTE_COUNT_LIMIT 1000
#define TRANSPORT_PACKAGES_CACHE 1000

#endif // CONFIG_H
