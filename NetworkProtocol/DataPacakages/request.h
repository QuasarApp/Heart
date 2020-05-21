#ifndef IREQUEST_H
#define IREQUEST_H

namespace NP {

/**
 * @brief The Request class - base request methods for data packages
 */
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
