#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <updateplayerdata.h>

namespace ClientProtocol {


class CLIENTPROTOCOLSHARED_EXPORT GameData: public UpdatePlayerData
{
private:
    QList<int> timeClick;
public:
    GameData();
    QList<int> getTimeClick() const;
    void setTimeClick(const QList<int> &value);
    int& operator[](int index);

    BaseNetworkObject *create() const override;
    NetworkClassSize classSize() const override;
    QDataStream &writeToStream(QDataStream &stream) const override;
    QDataStream &readFromStream(QDataStream &stream) override;
    bool isValid() const override;

};

}
#endif // GAMEDATA_H
