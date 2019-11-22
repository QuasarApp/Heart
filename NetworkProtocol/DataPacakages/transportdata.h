#ifndef TRANSPORTDATA_H
#define TRANSPORTDATA_H

#include "abstractdata.h"

#include <QHostAddress>


namespace NetworkProtocol {

template<typename T>
class NETWORKPROTOCOLSHARED_EXPORT TransportData: public AbstractData
{

public:
    TransportData();
    T *data() const;
    void setData(T *data);

    QHostAddress address() const;
    void setAddress(const QHostAddress &address);

private:
    T* _data = nullptr;
    QHostAddress _address;

    // StreamBase interface
public:
    QDataStream &fromStream(QDataStream &stream);
    QDataStream &toStream(QDataStream &stream) const;

    // AbstractData interface
protected:
    unsigned int generateId();

public:
    bool isValid() const;
};

template<typename T>
TransportData<T>::TransportData() {

}

template<typename T>
T *TransportData<T>::data() const {
    return _data;
}

template<typename T>
void TransportData<T>::setData(T *data) {
    _data = data;
}

template<typename T>
QHostAddress TransportData<T>::address() const {
    return _address;
}

template<typename T>
void TransportData<T>::setAddress(const QHostAddress &address) {
    _address = address;
}

template<typename T>
QDataStream &TransportData<T>::fromStream(QDataStream &stream) {
    stream >> _address;

    if (_data) {
        stream >> *_data;
    }

    return stream;
}

template<typename T>
QDataStream &TransportData<T>::toStream(QDataStream &stream) const {
    stream << _address;

    if (_data) {
        stream << *_data;
    }

    return stream;
}

template<typename T>
unsigned int TransportData<T>::generateId() {
    return qHash("TransportData");
}

template<typename T>
bool TransportData<T>::isValid() const {
    return _data && _data->isValid();
}

using Transport = TransportData<AbstractData>;
}
#endif // TRANSPORTDATA_H
