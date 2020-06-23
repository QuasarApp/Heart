#include "abstractkey.h"

unsigned int NP::AbstractKey::hash() const  {
    return _customHash;
}

void NP::AbstractKey::setCustomHash(unsigned int customHash) {
    _customHash = customHash;
}
