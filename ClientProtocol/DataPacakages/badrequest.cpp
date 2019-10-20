#include "badrequest.h"

ClientProtocol::BadRequest::BadRequest():AbstractData() {

}

ClientProtocol::BadRequest::BadRequest(const ClientProtocol::Package &package):
    AbstractData(package) {

}
