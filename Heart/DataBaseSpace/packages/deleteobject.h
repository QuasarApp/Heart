#ifndef DELETEOBJECT_H
#define DELETEOBJECT_H

#include <dbobject.h>


namespace QH {
namespace PKG {

/**
 * @brief The DeleteObject class is request for update object with dbId;
 */
class DeleteObject: public DBObject
{
public:
    DeleteObject();
    DeleteObject(const Package& pkg);

    // DBObject interface
public:
    DBObject *createDBObject() const override;

protected:
    BaseId generateId() const override;
    DBVariantMap variantMap() const override;

    // DBObject interface
public:
    bool isCached() const override;
};
}
}
#endif // DELETEOBJECT_H
