#ifndef DATASENDER_H
#define DATASENDER_H

#include <QObject>

class QAbstractSocket;

namespace NP {

/**
 * @brief The DataSender class - this class create a queue for sendet data to network.
 *  work on a own thread
 */
class DataSender: public QObject
{
    Q_OBJECT
public:
    DataSender();

public slots:
    /**
     * @brief sendPackagePrivate
     * @param array
     * @param target
     */
    void sendPackagePrivate(QByteArray array, void *target) const;
};
}
#endif // DATASENDER_H
