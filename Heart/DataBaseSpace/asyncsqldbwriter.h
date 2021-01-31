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
 * @brief The AsyncSqlDbWriter class is some as SqlDBWriter bud run all commnad in own thread
 * This class is thread save.
 */
class AsyncSqlDbWriter final : public SqlDBWriter
{
    Q_OBJECT
public:
    AsyncSqlDbWriter(QObject* ptr = nullptr);
    ~AsyncSqlDbWriter();

private:
    QThread *_own = nullptr;

};

}
#endif // ASYNCSQLDBWRITER_H
