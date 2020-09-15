#ifndef KNOWADDRESSES_H
#define KNOWADDRESSES_H

#include <QSet>
#include <abstractdata.h>
#include <baseid.h>

namespace QH {

/**
 * @brief The KnowAddresses class - this class is package for send the list of know addresses of node to other node object.
 */
class HEARTSHARED_EXPORT KnowAddresses: public AbstractData
{
public:
    KnowAddresses();
    KnowAddresses(const Package &pkg);

    // AbstractData interface
    bool isValid() const override;
    bool copyFrom(const AbstractData *) override;

    QSet<BaseId> knowAddresses() const;
    void setKnowAddresses(const QSet<BaseId> &knowAddresses);


    // StreamBase interface
protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    QSet<BaseId> _knowAddresses;
};
}
#endif // KNOWADDRESSES_H
