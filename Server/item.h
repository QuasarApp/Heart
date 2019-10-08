#ifndef ITEM_H
#define ITEM_H
#include <clientprotocol.h>
#include "server_global.h"

class SERVERSHARED_EXPORT Item : public ClientProtocol::Package
{
private:

    int id = -1;

public:
    Item();
    Item(const ClientProtocol::Package& other);
    Item(const ClientProtocol::BaseNetworkObject* obj);
    Item(ClientProtocol::Command cmd, const QByteArray& data);

    ~Item() override;
    ClientProtocol::Command cmd() const;
    const QByteArray& dataArray() const;
    bool isValid() const override;
    int getId() const;
    bool setId(int value);
    template<class T>
    bool parse(T& res) {
        return ClientProtocol::Package::parse(res);
    }
};

#endif // ITEM_H
