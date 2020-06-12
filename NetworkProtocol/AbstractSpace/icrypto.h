#ifndef ICRYPTO_H
#define ICRYPTO_H

#include <QList>
#include <QThread>
#include "networkprotocol_global.h"

class QMutex;

namespace NP {

class CryptoPairKeys;

/**
 * @brief The ICrypto class provide cryptografu functionality
 */
class NETWORKPROTOCOLSHARED_EXPORT ICrypto : public QThread
{
    Q_OBJECT
public:
    ICrypto();
    ~ICrypto();

    /**
     * @brief getNextPair - take a one pair key from keys pool.
     * @warning If key pool is empty then this method frease a current thread for awiting f neg generated pair key.
     * @note if the key is not generated within the specified period of time, an invalid copy of the key pair will be returned.
     * @param timeout_msec - timeout in milisecunds. default is 30000
     * @return pair of keys.
     */
    NP::CryptoPairKeys getNextPair(int timeout_msec = 30000);

    /**
     * @brief getKeyPoolSize
     * @return
     */
    int getKeyPoolSize() const;

    /**
     * @brief setKeyPoolSize
     * @param keyPoolSize
     */
    void setKeyPoolSize(int keyPoolSize);

protected:
    /**
     * @brief generate a new key. Default implementation do nothing.
     * @note Override this method for create of new class with new keys type.
     * @return
     */
    virtual CryptoPairKeys generate() const;

    void run() override;
private:

    /**
     * @brief waitForGeneratekey
     * @param timeout
     * @return
     */
    bool waitForGeneratekey(int timeout = 30000) const;

    QList<CryptoPairKeys> _keys;
    int _keyPoolSize = 1;

    QMutex *_keyPoolSizeMutex = nullptr;
    QMutex *_keysMutex = nullptr;


};

}
#endif // CRYPTO_H
