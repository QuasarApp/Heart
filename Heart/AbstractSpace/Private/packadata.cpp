#include "packadata.h"
#include "package.h"

namespace QH {

PackaData::~PackaData() {
    delete _data;
}
}
