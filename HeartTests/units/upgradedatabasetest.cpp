#include "upgradedatabasetest.h"

#include <QSqlQuery>
#include <QSqlResult>
#include <iobjectprovider.h>
#include <QtTest>
#include <isqldb.h>
#include <database.h>
#include <qaglobalutils.h>

#define LOCAL_TEST_PORT TEST_PORT + 5

class UpgradableDatabase: public QH::DataBase {


    // DataBaseNode interface
public:

    UpgradableDatabase() {
        initDBPatches();
    }

    bool checkVersion(int version) {
        QSqlQuery query;
        if (!db()->doQuery("SELECT * FROM DataBaseAttributes WHERE name='version'", {},  true, &query)){
            return false;
        };

        if (!query.next()) {
            return false;
        }

        int ver = query.value("value").toInt();
        return ver == version;
    }

protected:

    void initDBPatches() {

        addDBPatch({
                       0, // from version
                       1, // to version

                       [](const QH::iObjectProvider* database) -> bool {
                           QSqlQuery query;
                           if (!database->doQuery("select * from DataBaseAttributes", {}, true, &query)){
                               return false;
                           };

                           return true;
                       } // action of patch
                   });

        addDBPatch({
                       1, // from version
                       2, // to version
                       [](const QH::iObjectProvider* database) -> bool {
                           QSqlQuery query;
                           if (!database->doQuery("select * from DataBaseAttributes", {}, true, &query)){
                               return false;
                           };

                           return true;
                       }
                   });

        addDBPatch({
                       2, // from version
                       3, // to version
                       [](const QH::iObjectProvider* database) -> bool {
                           QSqlQuery query;
                           if (!database->doQuery("select * from DataBaseAttributes", {}, true, &query)){
                               return false;
                           };

                           return true;
                       }
                   });
    }
};

UpgradeDataBaseTest::UpgradeDataBaseTest() {

}

void UpgradeDataBaseTest::test() {

    UpgradableDatabase *db = new UpgradableDatabase();

    QVERIFY(db->run("UpgradeDBTest"));
    QuasarAppUtils::Params::log("Run DataBase finished successful", QuasarAppUtils::Info);

    QVERIFY(db->checkVersion(3));
    QuasarAppUtils::Params::log("Version is valid", QuasarAppUtils::Info);

    db->stop();
    QuasarAppUtils::Params::log("Stop Db finished successful", QuasarAppUtils::Info);

    QVERIFY(db->run("UpgradeDBTest"));
    QuasarAppUtils::Params::log("try run again finished successful", QuasarAppUtils::Info);

    delete db;

    QuasarAppUtils::Params::log("The UpgradeDataBaseTest test passed.", QuasarAppUtils::Info);

}
