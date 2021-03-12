/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/


#ifndef KEYSTORAGE_H
#define KEYSTORAGE_H

#include <QList>
#include <QThread>
#include "heart_global.h"
#include <QByteArray>
#include <QHash>
#include <QSet>
#include "config.h"

#define RAND_KEY ""

class QMutex;


namespace QH {

class CryptoPairKeys;
class ICrypto;
/**
 * @brief The KeyStorage class provide the functionality of control crypto keys (generate, save and write).
 * The current implementation of the key storage is collected cryptokeys like files in a key storage location.
 *
 * Work scheme:
 *\image html KeyStorage.svg width=800px
 */
class KeyStorage: public QThread
{
    Q_OBJECT
public:
    /**
     * @brief KeyStorage
     * @param cryptoMethod This is pointer to any class inherited from ICrypto
     */
    KeyStorage(ICrypto* cryptoMethod);
    ~KeyStorage();

    /**
     * @brief getNextPair This method take a one pair key from the keys pool or generate a new keys pair if the keys pool is empty.
     * @warning If key pool is empty then this method frease a current thread for awaiting f neg generated pair key.
     * @note if the key is not generated within the specified period of time, an invalid copy of the key pair will be returned.
     * @param accsessKey  The byte array for get a accesses to key from storage.
     * @param genesis Set this params to empty for get random key pair or set the byte array for get a key pair for genesis array.
     * @param timeout_msec This is timeout in milisecunds. default is WAIT_TIME (30000)
     * @return pair of keys.
     */
    CryptoPairKeys getNextPair(const QString &accsessKey,
                               const QByteArray &genesis = RAND_KEY,
                               int timeout_msec = WAIT_TIME);

    /**
     * @brief getKeyPoolSize - This method return size of keyPool.
     *  By default it is 1.
     * @return value of keys pool size.
     */
    int getKeyPoolSize() const;

    /**
     * @brief setKeyPoolSize This method return the size of keys pool.
     *  and start new work for generation a new keys.
     * @param keyPoolSize This is a new size of pool.
     */
    void setKeyPoolSize(int keyPoolSize);

    /**
     * @brief isValid This method check keysStorage to valid.
     * @return true if the crypto object is valid.
     */
    virtual bool isValid() const;

    /**
     * @brief isInited This method returns true if the crypto object was initialized correctly.
     * @return true if the crypto object was initialized correctly.
     */
    virtual bool isInited() const;

    /**
     * @brief crypt This method crypted a data message using a publicKey.
     * @param data This is pointer to data array for crypting.
     * @note data has ben changed after call this method.
     * @param publicKey This is key for crypting data.
     * @return true if function finished successful.
     */
    bool crypt(QByteArray *data, const QByteArray& publicKey);

    /**
     * @brief decrypt This method decrypt a data message using a privateKey.
     * @param cryptedData - Pointer to data array for decrypting.
     * @note cryptedData Has been changed after call this method.
     * @param privateKey This is private key for decrypting data.
     * @return true if function finished successful.
     */
    bool decrypt(QByteArray *cryptedData, const QByteArray& privateKey);

    /**
     * @brief sign This method sign a message using a privateKey.
     * @param data This is pointer to data array for signed.
     * @note data has been changed after call this method.
     * @param privateKey This is key for signing a data message.
     * @return true if function finished successful.
     */
    bool sign(QByteArray* data, const QByteArray& privateKey);

    /**
     * @brief extractSign This method extract sign from signed byteArray.
     * @param data This is a signed message.
     * @return return array of sign.
     */
    QByteArray extractSign(const QByteArray& data);

    /**
     * @brief concatSign This method concat a data of message and signs of a message.
     * @param data This is message data.
     * @param sign This is a sign of message.
     * @return signed message.
     */
    QByteArray concatSign(const QByteArray& data, const QByteArray& sign);

    /**
     * @brief check This method fore validation signed message.
     * @param signedData This is signed message for checking.
     * @param publicKey This is public key for validation of message.
     * @return true if function finished successful and signedData is valid.
     */
    bool check(const QByteArray& signedData, const QByteArray& publicKey);

    /**
     * @brief setGenesisList - set genesis list for generation key pairs.
     *  After invoke this method for each all items will be generated a keys pair.
     */
    void setGenesisList(const QList<QByteArray> &list);

    /**
     * @brief storageLocation This method return a path of the storage location.
     * By default storageLocation is QStandardPaths::DataLocation/KeysStorage
     * @return path to storage location of crypto keys.
     */
    QString storageLocation() const;

    /**
     * @brief initStorageLocation This method set a new path for storage location of keys.
     * @param value This is a new path to storage location.
     */
    bool initStorageLocation(const QString &value);

    /**
     * @brief initDefaultStorageLocation These is some as initStorageLocation,
     *  but set default path.
     * @param dirName This is name of storage location. If This parametr weel be empty then
     *  storage location set default direction name. By default is name of crypto class.
     * By default path of storage is:
     * \code
     * QStandardPaths::AppDataLocation/crypto/dirName;
     * \endcode
     * @return true if the storage inited successful.
     */
    bool initDefaultStorageLocation(const QString& dirName = "");

    /**
     * @brief clearStorage - This method clear all storage.
     */
    void clearStorage() const;

protected:

    /**
     * @brief toStorage This method save key into local storage.
     * @param key This is key data value of keys pair.
     * @note override this method if you want to change storage location or method of save of keys.
     * @return true if key saved successful.
     */
    virtual bool toStorage(const QString &key) const;

    /**
     * @brief fromStorage This method load keys from local storage.
     * @param key This is key data value of keys pair.
     * @return true if key pair saved successful.
     */
    virtual bool fromStorage(const QByteArray& key);

    /**
     * @brief run This method start the key generator on own thread.
     */
    void run() override;

    /**
     * @brief stop This method stop generate keys.
     */
    void stop();
private:

    /**
     * @brief waitForGeneratekey.
     * @param timeout - maximum time for generation new key. by default = WAIT_TIME (30000)
     * @return true if key generated successful.
     */
    bool waitForGeneratekey(const QString &key, int timeout = WAIT_TIME) const;

    /**
     * @brief waitForThreadFinished
     * @param timeout
     * @return
     */
    bool waitForThreadFinished(int timeout = WAIT_TIME) const;

    /**
     * @brief waitFor - Base wait function.
     * @param checkFunc - This is lambda of check event.
     * @param timeout - Maximum time line of waiting of event.
     * @return true if event is checkFunc return true.
     */
    bool waitFor(const std::function<bool()>& checkFunc, int timeout) const;

    /**
     * @brief loadAllKeysFromStorage
     */
    void loadAllKeysFromStorage();

    /**
     * @brief saveStorage This method for saved keys in storage.
     * @return true if all keys has been saved in a storage.
     */
    bool saveStorage() const;

    /**
      * @brief genKey - this method add a new task for generate keys pair
      * @param accessKey - the byte array for get access of the keys pair.
      * @param genesis - The byte array for generate new key.
      * @note If access key well be empty then this method return false.
      * @note For generate random key use a RAND_KEY genesis or empty value.
      * @return true if task of generation a new pair keys added successful else false.
    */
    bool genKey(const QString &accessKey, const QByteArray& genesis = RAND_KEY);

    QHash<QString, CryptoPairKeys> _keys;
    QSet<CryptoPairKeys> _randomKeysPool;

    QHash<QString, QByteArray> _generateTasks;

    int _keyPoolSize = 1;

    QMutex *_keyPoolSizeMutex = nullptr;
    QMutex *_keysMutex = nullptr;
    QMutex *_taskMutex = nullptr;

    QString _storageLocation;

    bool _inited = false;
    bool _stopGenerator = false;

    ICrypto *_cryptoMethod = nullptr;

    void generateKeysByTasks();
    void generateRandomKeys();
};
}
#endif // KEYSTORAGE_H
