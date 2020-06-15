#ifndef QSECRETRSA2048_H
#define QSECRETRSA2048_H

#include "icrypto.h"

#include <QHash>

class QRSAEncryption;

namespace NP {

/**
 * @brief The QSecretRSA2048 class Use
 */
class NETWORKPROTOCOLSHARED_EXPORT QSecretRSA2048: public ICrypto
{
    Q_OBJECT

    // ICrypto interface
public:
    QSecretRSA2048();
    ~QSecretRSA2048() override;
    bool crypt(QByteArray *data, const QByteArray &publicKey) override;
    bool decrypt(QByteArray *cryptedData, const QByteArray &privateKey) override;
    bool sign(QByteArray *data, const QByteArray &privateKey) override;
    bool check(const QByteArray &signedData, const QByteArray &publicKey) override;

protected:
    CryptoPairKeys generate(const QByteArray& genesis = {}) const override;


private:
    QRSAEncryption *qtSecret = nullptr;
};
}
#endif // QSECRETRSA2048_H
