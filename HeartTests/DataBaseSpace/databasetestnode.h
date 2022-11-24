#ifndef DATABASETESTNODE_H
#define DATABASETESTNODE_H

#include <QString>

namespace QH {
class ISqlDB;
class SqlDBWriter;
}

template <class BASE, class CACHE, class WRITER>
class DataBaseTestNode: public BASE {

protected:
    void initDefaultDbObjects(QH::ISqlDB *cache,
                              QH::SqlDBWriter *writer) override {
        if (!writer) {
            writer = new WRITER();
        }

        if (!cache) {
            cache = new CACHE();
        }

        BASE::initDefaultDbObjects(cache, writer);

        return;
    }
};

#endif // DATABASETESTNODE_H
