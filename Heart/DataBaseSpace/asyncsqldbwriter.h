/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ASYNCSQLDBWRITER_H
#define ASYNCSQLDBWRITER_H

#include "sqldbwriter.h"

namespace QH {


/**
 * @brief The AsyncSqlDbWriter class is some as SqlDBWriter but run all command in own thread.
 * This class is thread save.
 */
class AsyncSqlDBWriter : public SqlDBWriter
{
    Q_OBJECT
public:
    AsyncSqlDBWriter(QObject* ptr = nullptr);
    ~AsyncSqlDBWriter();

};

}
#endif // ASYNCSQLDBWRITER_H
