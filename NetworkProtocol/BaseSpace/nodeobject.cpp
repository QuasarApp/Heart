#include "nodeobject.h"
namespace NP {

NodeObject::NodeObject():DBObject("Nodes") {

}

NodeObject::NodeObject(const Package &pkg):
    NodeObject() {
    fromBytes(pkg.toBytes());
}

DBObject *NodeObject::factory() {
    return new NodeObject;
}

bool NodeObject::select(QSqlQuery &q) {

}

bool NodeObject::save(QSqlQuery &q) const {

}

}
