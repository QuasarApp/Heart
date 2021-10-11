#include "upgradedatabasetest.h"
#include "databasenode.h"

#include <QSqlQuery>
#include <QSqlResult>
#include <iobjectprovider.h>
#include <QtTest>
#include <isqldbcache.h>
#define LOCAL_TEST_PORT TEST_PORT + 5

class UpgradableDatabase: public QH::DataBaseNode {


    // DataBaseNode interface
public:

    bool checkVersion(int version) {
        QSqlQuery query;
        if (!db()->doQuery("SELECT * FROM DataBaseAttributes WHERE name='version'", true, &query)){
            return false;
        };

        if (!query.next()) {
            return false;
        }

        int ver = query.value("value").toInt();
        return ver == version;
    }

protected:


    QH::DBPatchMap dbPatches() const {
        QH::DBPatchMap result;

        result += [](const QH::iObjectProvider* database) -> bool {
            QSqlQuery query;
            if (!database->doQuery("select * from DataBaseAttributes", true, &query)){
                return false;
            };

            return true;
        };

        result += [](const QH::iObjectProvider* database) -> bool {
            QSqlQuery query;
            if (!database->doQuery("select * from DataBaseAttributes", true, &query)){
                return false;
            };

            return true;
        };

        result += [](const QH::iObjectProvider* database) -> bool {
            QSqlQuery query;
            if (!database->doQuery("select * from DataBaseAttributes", true, &query)){
                return false;
            };

            return true;
        };

        return result;
    }
};

UpgradeDataBaseTest::UpgradeDataBaseTest() {

}

void UpgradeDataBaseTest::test() {

    UpgradableDatabase *db = new UpgradableDatabase();

    QVERIFY(db->run(TEST_LOCAL_HOST, LOCAL_TEST_PORT, "UpgradeDBTest"));

    QVERIFY(db->checkVersion(3));

    db->stop();
    QVERIFY(db->run(TEST_LOCAL_HOST, LOCAL_TEST_PORT, "UpgradeDBTest"));
    db->softDelete();
}
