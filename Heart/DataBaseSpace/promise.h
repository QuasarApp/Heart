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
    class PromisePrivate {
        bool _rejected = false;
        QMutex _rejectedMutex;
        QList<const std::function<void(const SOURCE&)>> actions;
        const QSharedPointer<SOURCE> _null = nullptr;
        friend class Promise<SOURCE>;
    };

public:
    Promise() {
        _p = new PromisePrivate();
    };
    ~Promise() {
        delete _p;
    };

    /**
     * @brief wait This method wait for invoke setValue or failed. return true if the invoked setValue.
     * @param time This is time in msec for awaiting for value.
     * @return True if invoked setValue else return false.
     */
    bool wait(int time = WAIT_TIME) {
        return waitFor([this](){
            return !_data.isNull() || _p->_rejected;
        }, time);
    }

    /**
     * @brief isSuccess This method return true if object has valid value.
     * @return true if object has valid value.
     */
    bool isSuccess() const {
        return !_p->_rejected && !_data.isNull();
    }

    /**
     * @brief reject invoke this method for set fail for this object.
     */
    void reject() {
        _p->_rejectedMutex.lock();
        _p->_rejected = _data.isNull();
        _p->_rejectedMutex.unlock();
    }

    /**
     * @brief value This method return value of This object.
     * @return shared pointer of the value. If This promise rejected return invalid pointer.
     */
    const QSharedPointer<SOURCE>& value() {

        if (_p->_rejected) {
            return _p->_null;
        }

        if (!wait()) {
            return _p->_null;

        }

        return _data;
    }

    /**
     * @brief setValue This method set a new value for this promise.
     * @param val This is new value.
     */
    void setValue(const SOURCE& val) {
        _p->_rejectedMutex.lock();
        if (!_p->_rejected) {
            _data = QSharedPointer<SOURCE>::create(val);
        }
        _p->_rejectedMutex.unlock();

        triggered();


    }

    /**
     * @brief setValue This method is import a pointer to data.
     * @warning Do not use this method if the import pointe may be deleted from another objects.
     * @param val This is impoer value.
     */
    void setValue(const SOURCE* val) {
        _p->_rejectedMutex.lock();
        if (!_p->_rejected) {
            _data = QSharedPointer<SOURCE>(val);
        }
        _p->_rejectedMutex.unlock();

        triggered();

    }

    /**
     * @brief setValue This is implementation is waraper for shared pointers..
     * @warning Do not use this method if the import pointe may be deleted from another objects.
     * @param val This is impoer value.
     */
    void setValue(const QSharedPointer<SOURCE>& val) {
        _p->_rejectedMutex.lock();
        if (!_p->_rejected) {
            _data = val;
        }
        _p->_rejectedMutex.unlock();

        triggered();
    }

    /**
     * @brief subscribe This method add action to the list for execute when The promise shange own status to successful.
     * @param subscriber This is action for execute after setValue of this object.
     * @return true if subscribet finished successful.
     */
    bool subscribe(const std::function<void(const SOURCE&)> &action) {
        QMutexLocker locker(&_p->_rejectedMutex);
        if (_p->_rejected) {
            return false;
        }

        _p->actions.push_back(action);
    }

private:
    void triggered() {
        for (const auto & action: _p->actions) {
            action(*_data.data());
        }
    }

    PromisePrivate *_p = nullptr;
    QSharedPointer<SOURCE> _data = nullptr;

};
}
#endif // PROMISE_H
