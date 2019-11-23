#ifndef IREQUEST_H
#define IREQUEST_H

namespace NetworkProtocol {

class Request
{
public:
    Request();
    unsigned char getRequestCmd() const;
    void setRequestCmd(unsigned char value);

protected:
    unsigned char requestCmd = 0;

};
}
#endif // IREQUEST_H
