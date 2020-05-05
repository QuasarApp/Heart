#ifndef CONFIG_H
#define CONFIG_H
#include <QStandardPaths>

#define LOCAL_SERVER    "127.0.0.1"

#define DEFAULT_PORT          3090

#define DEFAULT_DB_NAME "QtNetworkDB"
#define DEFAULT_DB_PATH QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/" + DEFAULT_DB_NAME
#define DEFAULT_DB_INIT_FILE_PATH ":/sql/default"
#define DEFAULT_UPDATE_INTERVAL 3600000 // 1 hour

#endif // CONFIG_H
