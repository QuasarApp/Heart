/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "senderdata.h"
namespace QH {

SenderData::SenderData() {

}

const NodeId &SenderData::senderID() const {
    return _senderID;
}

void SenderData::setSenderID(const NodeId &senderID) {
    _senderID = senderID;
}
}
