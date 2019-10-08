#ifndef LOGIN_H
#define LOGIN_H

#include "basenetworkobject.h"
namespace ClientProtocol {

class CLIENTPROTOCOLSHARED_EXPORT Login : public BaseNetworkObject
{

private:
    QByteArray hashRsaPass;
    QString gmail;
    bool registerNewUser = false;

public:
    Login();

    BaseNetworkObject *create() const override;
    NetworkClassSize classSize() const override;
    QDataStream &writeToStream(QDataStream &stream) const override;
    QDataStream &readFromStream(QDataStream &stream) override;
    bool isValid() const override;
    QString getGmail() const;
    void setGmail(const QString &value);
    QByteArray getHashPass() const;
    void setHashPass(const QByteArray &value);
    bool getRegisterNewUser() const;
    void setRegisterNewUser(bool value);
};
}
#endif // LOGIN_H
