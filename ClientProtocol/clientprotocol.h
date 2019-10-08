#ifndef CLIENTPROTOCOL_H
#define CLIENTPROTOCOL_H

#include "clientprotocol_global.h"

#include <QVariantMap>
#include <snakeutils.h>
#include "config.h"

#define BASE_ENCRYPTION_BITS 256
#define BASE_RSA_BITS static_cast<QRSAEncryption::Rsa>(BASE_ENCRYPTION_BITS)
#define BASE_HASH_BITS 256


namespace ClientProtocol {

class BaseNetworkObject;

enum class Type: quint8 {
    Undefined = 0x00,
    Responke = 0x01,
    Request = 0x02,
    Stream = 0x03,
};

enum class Command: quint8 {
    Undefined = 0x00,
    Ping = 0x01,
    BadRequest = 0x02,
    Login = 0x03,
    UpdatePlayerData = 0x04,
    GameData = 0x05,
    GetItem = 0x06,
    Player = 0x07,
    Snake = 0x08,
    Map = 0x09,
    PubKey = 0x0a,
    WebSocket = 0x0b
};

bool isValidSize(quint8 type, unsigned int size);

bool CLIENTPROTOCOLSHARED_EXPORT initClientProtockol();
auto cast(const BaseNetworkObject *obj);
/**
 * @brief The Header struct 4 byte
 */
#pragma pack(push, 1)
struct CLIENTPROTOCOLSHARED_EXPORT Header {
    /**
     * @brief size - size of package data (not header)
     */
    unsigned short size: 12;
    /**
     * @brief type of package see Type
     */
    quint8 type: 2;
    /**
     * @brief command of pacage see Command
     */
    quint8 command: 5;

    /**
     * @brief command of pacage see Command (rquest from client)
     * the server should write to which command it responds
     */
    quint8 requestCommand: 5;

    /**
     * @brief sig
     * signed of package (package number)
     */
    unsigned char sig : 8;


    /**
     * @brief Header default constructor
     */
    Header();

    /**
     * @brief isValid
     * @return true if header is valid
     */
    bool isValid() const;


    /**
     * @brief reset - reset all data and set for header invalid status
     */
    void reset();
};
#pragma pack(pop)

/**
 * @brief The Package struct
 */
struct CLIENTPROTOCOLSHARED_EXPORT Package {
    /**
     * @brief hdr - header of package
     */
    Header hdr;
    /**
     * @brief data - source data of package
     */
    QByteArray data;

    Package();

    /**
     * @brief isValid
     * @return true if package is valid
     */
    virtual bool isValid() const;

    /**
     * @brief parse
     * @return Qmap of package (default key if "value")
     */
    BaseNetworkObject * parse() const;

    template<class T>
    bool parse(T& res) {
        auto obj = static_cast<T*>(parse());

        if (!obj)
            return false;

        res = *obj;
        delete obj;

        return true;
    }

    /**
     * @brief create - fill package
     * @param data - data of filled
     * @param old header (only for request type)
     * @return true if all done
     */
    bool create(const BaseNetworkObject *data, Type type, const Header *old = nullptr);

    /**
     * @brief create
     * @param cmd command of package
     * @param type type
     * @param data - data of filled
     * @param old - header (only for request type)
     * @return true if all good
     */
    bool create(Command cmd, Type type, const QByteArray& data, const Header* old = nullptr);



    /**
     * @brief create
     * @param cmd command of package
     * @param type type
     * @param old header (only for request type)
     * @return true if all good
     */
    bool create(Command cmd, Type type, const Header* old = nullptr);

    /**
     * @brief toBytes
     * @return bytes array of packag
     */
    QByteArray toBytes() const;

    /**
     * @brief reset - reset all data and set for package invalid status
     */
    void reset();

    /**
     * @brief signPackage sign package from old data
     * @param oldHeader old data header
     * @return true if all good
     */
    bool signPackage(const Header* oldHeader);

    virtual ~Package() = default;

};

}

#endif // CLIENTPROTOCOL_H
