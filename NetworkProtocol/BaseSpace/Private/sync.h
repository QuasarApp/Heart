#ifndef SYNC_H
#define SYNC_H

namespace NP {

class BaseNode;

/**
 * @brief The NetworkStatus enum
 */
enum class NetworkStatus {
    /// the node on offline mode
    Offline,

    /// the node synced with all network
    Sync,

    /// the node hawe a actuale data of network database.
    Synced
};

/**
 * @brief The Sync class
 */
class Sync
{
public:
    explicit Sync(BaseNode *node);

    NetworkStatus status() const;
    void setStatus(const NetworkStatus &status);

private:
    NetworkStatus _status = NetworkStatus::Offline;
    BaseNode * _node = nullptr;
};
}
#endif // SYNC_H
