#ifndef DATASENDER_H
#define DATASENDER_H

#include <QObject>

class QAbstractSocket;

namespace QH {

/**
 * @brief The DataSender class this class create a queue for sendet data to network.
 *  work on a main thread
 */
class DataSender: public QObject
{
    Q_OBJECT
public:
    DataSender();

public slots:
    /**
     * @brief sendPackagePrivate This slot move send package to a main threan
     * @param array bytes to send
     * @param target - this is pointer of target socket
     */
    void sendPackagePrivate(QByteArray array, void *target) const;
};
}
#endif // DATASENDER_H
