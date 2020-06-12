#ifndef QSECRETRSA2048_H
#define QSECRETRSA2048_H

#include "icrypto.h"

#include <QHash>

namespace NP {

/**
 * @brief The QSecretRSA2048 class Use
 */
class NETWORKPROTOCOLSHARED_EXPORT QSecretRSA2048: public ICrypto
{
    Q_OBJECT
public:
    QSecretRSA2048(const QList<QByteArray>& genesises);
    CryptoPairKeys getFromGenesis(const QByteArray& genesis);
    // ICrypto interface
protected:
    CryptoPairKeys generate() const override;
    void run() override;

private:
    QHash <QByteArray, CryptoPairKeys > _genesisKeys;
};
}
#endif // QSECRETRSA2048_H
