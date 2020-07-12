#ifndef ICRYPTO_H
#define ICRYPTO_H

#include <QList>
#include <QThread>
#include "networkprotocol_global.h"
#include <QByteArray>
#include <QHash>
#define RAND_KEY "QNP_RAND_KEY"

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
     * @param accsessKey - the byte array for get a acceses to key from storage.
     * @param genesis - set this params to empty for get random key pair or set the byte array for get a key pair for genesis array.
     * @param timeout_msec - timeout in milisecunds. default is 30000
     * @return pair of keys.
     */
    CryptoPairKeys getNextPair(const QByteArray& accsessKey,
                               const QByteArray &genesis = RAND_KEY,
                               int timeout_msec = 30000);

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

    /**
     * @brief crypt
     * @param data - pointer to data array for crypting.
     * @note data as ben changed after call this method.
     * @param publicKey - key for crypting data
     * @return true if function finished seccussful
     */
    virtual bool crypt(QByteArray *data, const QByteArray& publicKey) = 0;

    /**
     * @brief decrypt
     * @param cryptedData - pointer to data array for decrypting.
     * @note cryptedData as ben changed after call this method.
     * @param privateKey
     * @return true if function finished seccussful
     */
    virtual bool decrypt(QByteArray *cryptedData, const QByteArray& privateKey) = 0;

    /**
     * @brief sign
     * @param data - pointer to data array for signed.
     * @note data as ben changed after call this method.
     * @param privateKey
     * @return true if function finished seccussful
     */
    virtual bool sign(QByteArray* data, const QByteArray& privateKey) = 0;

    /**
     * @brief extractSign - extract sign from signed byteArray
     * @param data - signed message.
     * @return return array of sign.
     */
    virtual QByteArray extractSign(const QByteArray& data) = 0;

    /**
     * @brief concatSign
     * @param data - message data
     * @return signed message
     */
    virtual QByteArray concatSign(const QByteArray& data, const QByteArray& sign) = 0;

    /**
     * @brief check
     * @param publicKey
     * @return true if function finished seccussful and signedData is valid.
     */
    virtual bool check(const QByteArray& signedData, const QByteArray& publicKey) = 0;

    /**
     * @brief setGenesisList - set genesis list for generation key pairs
     */
    void setGenesisList(const QList<QByteArray> &list);

    /**
     * @brief storageLocation
     * @default QStandardPaths::DataLocation/KeysStorage
     * @return parth to storage location of crypto keys
     */
    QString storageLocation() const;

    /**
     * @brief setStorageLocation set a new path for storage location of keys.
     * @param value - new path
     */
    void setStorageLocation(const QString &value);

protected:

    /**
     * @brief toStorage - save key from genesis into local storage.
     * @param genesis - genesis of key pair
     * @note override this method if you want to change storage location or method of save of keys.
     * @return true if key saved successful
     */
    virtual bool toStorage(const QByteArray& genesis);

    /**
     * @brief fromStorage - load keys from local storage
     * @param genesis - genesis of key pair
     * @return true if key pair saved seccussful.
     */
    virtual bool fromStorage(const QByteArray& genesis);


    /**
     * @brief generate a new key. Default implementation do nothing.
     * @note Override this method for create of new class with new keys type.
     * @param genesis
     * @return crypto pair keys
     */
    virtual CryptoPairKeys generate(const QByteArray& genesis = {}) const = 0;

    void run() override;
private:

    /**
     * @brief waitForGeneratekey
     * @param timeout
     * @return
     */
    bool waitForGeneratekey(const QByteArray &genesis = RAND_KEY, int timeout = 30000) const;

    /**
     * @brief loadAllKeysFromStorage
     */
    void loadAllKeysFromStorage();

    /**
      * @brief genKey - this method add a new task for generate keys pair
      * @param genesis - the byte array for generate new key
      * @param accessKey - the byte array for get access of the keys pair.
      * @note If the access key well be empty then accessKey = genesis.
      *  If access key and genesis well be empty then this method return false.
      * @return true if task of generation a new pair keys added seccussful else false.
    */
    bool genKey(const QByteArray& genesis, QByteArray accessKey = {});

    /**
     * @brief genRandomKey - generate a new random pair key
     * @param accessKey - the byte array for get access of the keys pair.
     * @return return true if task for generate new key pair added succesful.
     */
    bool genRandomKey(const QByteArray& accessKey);

    QHash<QByteArray, QList<CryptoPairKeys>> _keys;
    QHash<QByteArray, QByteArray> _generateTasks;

    int _keyPoolSize = 1;

    QMutex *_keyPoolSizeMutex = nullptr;
    QMutex *_keysMutex = nullptr;
    QMutex *_taskMutex = nullptr;

    QString _storageLocation;

};

}
#endif // CRYPTO_H
