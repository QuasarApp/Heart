#ifndef PLAYER_H
#define PLAYER_H

#include "updateplayerdata.h"


namespace ClientProtocol {


class CLIENTPROTOCOLSHARED_EXPORT Player: public UpdatePlayerData
{
private:
    QString name = "user";
    QString gmail = "";

    unsigned int money = 0;
    unsigned int record = 0;
    unsigned int avgRecord = 0;
    QList<int> items;
    int cureentSnake = -1;

public:
    Player();

    BaseNetworkObject *create() const override;
    NetworkClassSize classSize() const override;
    QDataStream &writeToStream(QDataStream &stream) const override;
    QDataStream &readFromStream(QDataStream &stream) override;
    bool isValid() const override;
    QString getName() const;
    void setName(const QString &value);
    QString getGmail() const;
    void setGmail(const QString &value);
    unsigned int getMany() const;
    void setMany(unsigned int value);
    unsigned int getRecord() const;
    void setRecord(unsigned int value);
    QList<int> getItems() const;
    void setItems(const QList<int> &value);
    int getCureentSnake() const;
    void setCureentSnake(int value);
    unsigned int getAvgRecord() const;
    void setAvgRecord(unsigned int value);
//    QString getHexPass() const;
//    void fromHexPass(const QString& passHex);

};

}
#endif // PLAYER_H
