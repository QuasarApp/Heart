#ifndef SNAKE_H
#define SNAKE_H

#include "basenetworkobject.h"


namespace ClientProtocol {

class CLIENTPROTOCOLSHARED_EXPORT Snake: public BaseNetworkObject
{
private:
    quint8 speed;
    quint8 snakeClass;
    QList<float> skillet;

public:
    Snake();
    quint8 getSpeed() const;
    void setSpeed(const quint8 &value);
    quint8 getSnakeClass() const;
    void setSnakeClass(const quint8 &value);
    QList<float> getSkillet() const;
    void setSkillet(const QList<float> &value);

    BaseNetworkObject *create() const override;
    NetworkClassSize classSize() const override;
    QDataStream &writeToStream(QDataStream &stream) const override;
    QDataStream &readFromStream(QDataStream &stream) override;
    bool isValid() const override;
};
}
#endif // SNAKE_H
