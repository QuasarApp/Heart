#ifndef NETWORKREQUEST_H
#define NETWORKREQUEST_H

#include <QSet>
#include <abstractdata.h>
#include <baseid.h>


namespace QH {
namespace PKG {

/**
 * @brief The NetworkRequest class - this is pakcage wraper for sending requests to network.
 * this packages generated when current node can not process a request from client node.
 * This request moving by network while do not get a ansver.
 * The Ansver (or Responce) it is package with all needed data for processing of a request.
 */
class HEARTSHARED_EXPORT NetworkRequest: public AbstractData
{
public:
    explicit NetworkRequest();
    explicit NetworkRequest(const Package& package);

    // AbstractData interface
    /**
     * @brief isValid chck this package
     * @return true if package is valid
     */
    bool isValid() const override;

    /**
     * @brief copyFrom - create a copy of this pacakge
     * @return
     */
    bool copyFrom(const AbstractData *) override;

    /**
     * @brief dataRequest - this is package of not processed reqest.
     * @return Package of request
     */
    Package dataRequest() const;

    /**
     * @brief setDataRequest - set Data of Reqest Package.
     * @param data new pakcage.
     */
    void setDataRequest(const Package &data);

    /**
     * @brief dataResponce - this is package of responce of request.
     *  The Responce package contains all needed data for procesing a request.
     * @return pakage of Responce
     */
    Package dataResponce() const;

    /**
     * @brief setDataResponce - set Data of Responce package.
     * @note If this package contains a responce of request then package is completed.
     * @param data - is Responce pacakge
     */
    void setDataResponce(const Package &data);

    /**
     * @brief isComplete - return complete status. If this package contains a responce of request then package is completed.
     * @return true if package is complete.
     */
    bool isComplete() const;

    /**
     * @brief addNodeRequiringData - add a node to the list of nodes that need a responce of the request
     * @param node - id of node
     */
    void addNodeRequiringData(const BaseId& node);

    /**
     * @brief askedNodes - return list of nodes that need a responce of the request
     * @return list of nodes id.
     */
    QSet<BaseId> askedNodes() const;

    /**
     * @brief removeNodeFromAskedList - remove a node from list of nodes that need a responce of the request.
     *  Invoke this method when the node will get a responce.
     * @param node
     */
    void removeNodeFromAskedList(const BaseId& node);

protected:
    QDataStream &fromStream(QDataStream &stream) override;
    QDataStream &toStream(QDataStream &stream) const override;

private:
    Package _dataRequest;
    QSet<BaseId> _askedNodes;
    Package _dataResponce;

};
}
}
#endif // NETWORKREQUEST_H
