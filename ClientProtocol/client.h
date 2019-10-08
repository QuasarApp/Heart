#ifndef CLIENT_CP_H
#define CLIENT_CP_H

#include "clientprotocol_global.h"
#include "clientprotocol.h"
#include <QObject>
#include <QSet>

class QTcpSocket;
class Player;
class BaseItem;
class testSankeServer;

namespace ClientProtocol {


class CLIENTPROTOCOLSHARED_EXPORT Client: public QObject
{
    Q_OBJECT
private:
    QTcpSocket *_destination;
    Package _downloadPackage;
    bool _online = false;
    bool _logined = false;
    QByteArray _token;
    QByteArray _rsaKey;
    int currentIndex = 0;
    QHash<unsigned char, QVariantMap> _requestsMap;
    QHash<quint8, bool> _subscribe; // command and data confirmation
    QString _address = LOCAL_SNAKE_SERVER;
    unsigned short _port = DEFAULT_SNAKE_PORT;

    /**
     * @brief checkCommand - return old sendet command if commnad not valid return undefined command
     * @param sig - sig package
     * @param reqCmd - reqCmd of package
     * @param type - type of package
     * @return if commnad not valid return undefined command
     */
    Command checkCommand(int sig, Command reqCmd, Type type);
    bool receiveData(const QByteArray &obj, Header hdr);
    bool setRSAKey(const QByteArray &key);
    void setLoginStatus(bool newStatus);
    void setOnlineStatus(bool newStatus);
    bool sendPackage(Package &pkg);
    inline unsigned char nextIndex();


    QByteArray generateHash(const QByteArray &pass) const;

    void updateStatuses(Command extCmd, Command cmd, Type type, const QByteArray &obj);

private slots:
    void incommingData();
    void handleDisconnected();

protected:
    int _currentUserId = -1;


public:
    explicit Client(const QString& addrress = LOCAL_SNAKE_SERVER,
                    unsigned short port = DEFAULT_SNAKE_PORT,
                    QObject * ptr = nullptr);


    /**
     * @brief login - register or login a user
     * @param gmail - gmail of user
     * @param pass - hash of pass of user
     * @param error - error ( if all good this variable is empty)
     * @return true if message sendet
     */
    bool virtual login(const QString& gmail, const QByteArray &pass, bool newUser = false);

    /**
     * @brief registration - it is wraper of login method
     * @return true if message sendet
     */
    bool virtual registration(const QString& gmail, const QByteArray &pass);


    bool ping();

    void loginOut();
    void dissconnectFromHost();
    void connectToHost(const QString &addrress, unsigned short port);
    void reconnectToHost();

    /**
     * @brief updateData
     * @return  true if refresh seсcessfully completed
     */
    bool updateData();

    /**
     * @brief savedata
     * @param gameData - data of lvl
     * @return true if all good
     */
    bool savaData(const QList<int> &gameData);

    /**
     * @brief getItem
     * @param id of item
     * @return item data
     */
    bool getItem(int id);

    /**
     * @brief getPlayer
     * @param id of player
     * @return player data
     */
    bool getPlayer(int id);

    /**
     * @brief isOnline
     * @return true if client is connected to server and get rsapub key
     */
    const bool &isOnline() const;

    /**
     * @brief isLogin
     * @return true if player is online
     */
    const bool &isLogin() const;

    /**
     * @brief setSubscribe change subscribe of command "cmd"
     * @param cmd - command of subscribe
     * @param subscribe - boolean barametr. true is subscribe, false is unsubscribe
     * @param id - id of object for commands (Player and Item).
     *  If this parameter is -1 then subscribe on all changes of objects.
     * @return true if all good
     */
    bool setSubscribe(Command cmd, bool subscribe, int id = -1);

    /**
     * @brief getSubscribe
     * @return set of current subscribe commands
     */
    QHash<quint8, bool> getSubscribe() const;

    friend class ::testSankeServer;


signals:
    void sigIncommingData(Command cmd, const QByteArray& obj);
    void onlineChanged(bool);
    void loginChanged(bool);

};

}
#endif // CLIENT_CP_H
