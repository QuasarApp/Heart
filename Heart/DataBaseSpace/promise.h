#ifndef PROMISE_H
#define PROMISE_H

#include <QSharedPointer>



namespace QH {

/**
 * @brief The Promise class This is simple implementation Promise for the Qt5.
 * @note In The Qt6 use QPromise class.
 */
template <class SOURCE>
class Promise
{
public:
    Promise();
    ~Promise();

    /**
     * @brief wait This method wait for invoke setValue or failed. return true if the invoked setValue.
     * @param time This is time in msec for awaiting for value.
     * @return True if invoked setValue else return false.
     */
    bool wait(int time = 30000);

    /**
     * @brief isSuccess This method return true if object has valid value.
     * @return true if object has valid value.
     */
    bool isSuccess() const;

    /**
     * @brief reject invoke this method for set fail for this object.
     */
    void reject();

    /**
     * @brief value This method return value of This object.
     * @return shared pointer of the value. If This promise rejected return invalid pointer.
     */
    QSharedPointer<SOURCE>& value();

    /**
     * @brief setValue This method set a new value for this promise.
     * @param val This is new value.
     */
    void setValue(const SOURCE& val);

    /**
     * @brief subscribe This method add action to the list for execute when The promise shange own status to successful.
     * @param subscriber This is action for execute after setValue of this object.
     */
    void subscribe(const std::function<void(const SOURCE&)>& action);

private:
    QSharedPointer<SOURCE> _data;
};
}
#endif // PROMISE_H
