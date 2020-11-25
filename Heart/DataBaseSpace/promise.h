#ifndef PROMISE_H
#define PROMISE_H

#include <QMutex>
#include <QSharedPointer>

#include "config.h"

namespace QH {

/**
 * @brief waitFor This is function for for wait a condition.
 * @param condition This is awaiting condition
 * @param timeout This is limit of rime in msecs.
 * @return true if thew condition is true.
 */
bool waitFor(const std::function<bool()> &condition, int timeout = WAIT_TIME);

/**
 * @brief The Promise class This is simple implementation Promise for the Qt5.
 * @note In The Qt6 use QPromise class.
 */
template <class SOURCE>
class Promise
{
    enum State: char {
        Unknown,
        Succsess,
        Reject
    };

    class PromisePrivate {
        State _state = Unknown;
        QMutex _mutex;
        QList<const std::function<void(SOURCE&)>> actions;
        SOURCE _data;

        friend class Promise<SOURCE>;
    };

public:
    Promise() {
        _p = new PromisePrivate();
    }

    ~Promise() {
        if (_p)
            delete _p;
    }

    /**
     * @brief wait This method wait for invoke setValue or failed. return true if the invoked setValue.
     * @param time This is maximum awaiting time in msec.
     * @return True if invoked setValue else return false.
     */
    bool wait(int time = WAIT_TIME) {
        return waitFor([this](){
            return _p->_state;
        }, time);
    }

    /**
     * @brief waitSuccess This method return true if promise finished succesful.
     * @param time This is maximum awaiting time in msec.
     * @return true if promise finished succesful.
     */
    bool waitSuccess(int time = WAIT_TIME) {
        return wait(time) && _p->_state == Succsess;
    }

    /**
     * @brief isSuccess This method return true if object has valid value.
     * @return true if object has valid value.
     */
    bool isSuccess() const {
        return _p->_state == Succsess;
    }

    /**
     * @brief reject invoke this method for set fail for this object.
     */
    void reject() {
        _p->_mutex.lock();
        _p->_state = Reject;
        _p->_mutex.unlock();
    }

    /**
     * @brief value This method return the refernce to the promise data.
     * If promise is rejected or not completed then return invalid value.
     * @return value of the promise.
     */
    const SOURCE& value() const {
        return _p->_data;
    }

    /**
     * @brief setValue This method set a new value for this promise.
     * @param val This is new value.
     */
    void setValue(const SOURCE& val) {
        if (!isFinished()) {

            _p->_mutex.lock();
            _p->_data =val;
            _p->_state = Succsess;
            _p->_mutex.unlock();

            triggered();

        }
    }

    /**
     * @brief subscribe This method add action to the list for execute when The promise shange own status to successful.
     * @param subscriber This is action for execute after setValue of this object.
     * @return true if subscribet finished successful.
     */
    bool subscribe(const std::function<void(SOURCE&)> &action) {
        QMutexLocker locker(&_p->_mutex);
        if (_p->_state == Reject) {
            return false;
        }

        if (_p->_state == Succsess) {
            action(_p->_data);
            return true;
        }

        _p->actions.push_back(action);
        return true;
    }

    /**
     * @brief isFinished This method return true if this promise is complete. (rejected or seted value)
     * @return true if this object is completed.
     */
    bool isFinished() {
        return _p->_state;
    }

private:
    void triggered() {
        for (const auto & action: _p->actions) {
            action(_p->_data);
        }
    }

    PromisePrivate *_p = nullptr;

};
}
#endif // PROMISE_H
