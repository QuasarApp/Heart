#include "deleteobjectrequest.h"
namespace NP {

DeleteObjectRequest::DeleteObjectRequest() {
    INIT_COMMAND

}

DeleteObjectRequest::DeleteObjectRequest(const DbAddress &address):
    DeleteObjectRequest() {

    setAddress(address);
}
}
