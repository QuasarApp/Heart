#include "sqldbwriter.h"

#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <quasarapp.h>
#include <clientprotocol.h>
#include <QJsonDocument>
#include <QJsonObject>

namespace ClientProtocol {


bool SqlDBWriter::exec(QSqlQuery *sq,const QString& sqlFile) {
    QFile f(sqlFile);
    bool result = true;
    if (f.open(QIODevice::ReadOnly)) {

        QString temp, delimiter = ";";
        QTextStream stream(&f);
        stream.setCodec("UTF8");

        while(!stream.atEnd()) {

            temp += stream.readLine();

            if (temp.lastIndexOf("delimiter", -1, Qt::CaseInsensitive) > -1) {

                temp.remove("delimiter", Qt::CaseInsensitive);

                int last = temp.indexOf(QRegularExpression("[^ \f\n\r\t\v]")) + 1;

                int begin = temp.lastIndexOf(QRegularExpression("[^ \f\n\r\t\v]"));

                delimiter = temp.mid(begin, last - begin);

                temp = "";
            } else {
                if (temp.lastIndexOf(delimiter) >- 1) {
                    temp.remove(delimiter);

                    result = result && sq->exec(temp);

                    if (!result) {
                        qCritical() << sq->lastError().text();
                        f.close();
                        return false;
                    }

                    if (temp.contains("CREATE TABLE ", Qt::CaseInsensitive)) {
                        int tableBegin = temp.indexOf('(');
                        int tableEnd = temp.indexOf(')');

                        QStringList columns = temp.mid(tableBegin, tableEnd - tableBegin).split(',');

                        for (QString & col : columns) {
                            getType(col);
                        }

                    }

                    temp = "";
                }
            }
        }

        f.close();
        return result;
    }
    return false;
}

bool SqlDBWriter::enableFK() {

    QString request = QString("PRAGMA foreign_keys = ON");
    if (!query.exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query.lastError().text());
        return false;
    }

    return true;
}

bool SqlDBWriter::disableFK() {

    QString request = QString("PRAGMA foreign_keys = OFF");
    if (!query.exec(request)) {
        QuasarAppUtils::Params::verboseLog("request error : " + query.lastError().text());
        return false;
    }

    return true;

}

QVariantMap SqlDBWriter::getInitParams(const QString &initFile) const {
    QFile file(initFile);
    QVariantMap res;

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        if (!doc.isObject()) {
            return res;
        }

        QJsonObject obj = doc.object();
        return obj.toVariantMap();
    }

    return res;
}

/*
 *  about driver see https://doc.qt.io/qt-5/sql-driver.html
 */
QVariantMap SqlDBWriter::defaultInitPararm() const {
    QVariantMap params;
    params["DBDriver"] = "QSQLITE";
    params["DBFile"] = DEFAULT_DB_PATH;

    return params;
}

QHash<QString, IDbTable *> SqlDBWriter::getDbStruct() const
{
    return _dbStruct;
}

SqlDBWriter::SqlDBWriter() {
}

bool SqlDBWriter::initDb(const QString &initDbParams) {

    QVariantMap params;

    if (initDbParams.isEmpty()) {
        params = defaultInitPararm();
    } else {
        params = getInitParams(initDbParams);
    }

    db = QSqlDatabase::addDatabase(params["DBDriver"].toString(),
            QFileInfo(params["DBFilePath"].toString()).fileName());

    if (params.contains("DBFilePath")) {

        auto path = QFileInfo(params["DBFilePath"].toString()).absoluteFilePath();
        if (!QDir("").mkpath(path)) {
            return false;
        }

        db.setDatabaseName(path);
    }

    if (params.contains("DBLogin")) {
        db.setPassword(params["DBLogin"].toString());
    }

    if (params.contains("DBPass")) {
        db.setPassword(params["DBPass"].toString());
    }

    if (params.contains("DBHost")) {
        db.setHostName(params["DBHost"].toString());
    }

    if (params.contains("DBPort")) {
        db.setPort(params["DBPort"].toInt());
    }

    query = QSqlQuery(db);

    if (!db.open()) {
        return false;
    }

    if (params.contains("DBInitFile")) {
        auto path = QFileInfo(params["DBInitFile"].toString()).absoluteFilePath();

        if (!exec(&query, path)) {
            return false;
        }
    }

    initSuccessful = db.isValid();
    return initSuccessful;
}

bool SqlDBWriter::isValid() const {
    return db.isValid() && db.isOpen() && initSuccessful;
}

SqlDBWriter::~SqlDBWriter() {
}


#define c(x) str.contains(x, Qt::CaseInsensitive)
QVariant::Type SqlDBWriter::getType(const QString &str) {

    if (str.isEmpty() || c(" BINARY") || c(" BLOB") || c(" TINYBLOB") || c(" MEDIUMBLOB") || c(" LONGBLOB")) {
        return QVariant::ByteArray;
    } else if (c(" INT")) {
        return QVariant::Int;
    } else if (c(" VARCHAR") || c(" TEXT") || c(" TINYTEXT") || c(" MEDIUMTEXT") || c(" LONGTEXT")) {
        return QVariant::String;
    } else if (c(" FLOAT") || c(" DOUBLE") || c(" REAL")) {
        return QVariant::Double;
    } else if (c(" BOOL")) {
        return QVariant::Bool;
    } else if (c(" DATETIME")) {
        return QVariant::DateTime;
    } else if (c(" DATE")) {
        return QVariant::Date;
    } else if (c(" TIME")) {
        return QVariant::Time;
    }

    return QVariant::ByteArray;
}

}
