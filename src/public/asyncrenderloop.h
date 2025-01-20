/*
 * Copyright (C) 2025-2025 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ASYNCRENDERLOOP_H
#define ASYNCRENDERLOOP_H

#include "async.h"

namespace QH {

/**
 * @brief The AsyncRenderLoop class is a class for rendering the world.
 */
class HEARTSHARED_EXPORT AsyncRenderLoop: public Async
{
    Q_OBJECT
public:
    AsyncRenderLoop(QThread* thread, QObject* ptr = nullptr);

    /**
     * @brief run This method starts the render loop.
     */
    virtual void run();

    /**
     * @brief stop This method stops the render loop.
     */
    virtual void stop();

    /**
     * @brief isRun This method returns the state of the render loop.
     * @return true if the render loop is running, else false.
     */
    bool isRun() const;

protected:

    /**
     * @brief renderIteration This method is called in each iteration of the render loop.
     * This method must be implemented in the derived class.
     * @param msec time in milliseconds from the last iteration.
     * @see stop
     * @see run
     */
    virtual void renderIteration(int msec) = 0;

private slots:
    void renderLoopPrivate();

private:
    bool m_run = false;
    quint64 _lastIterationTime = 0;

};
}
#endif // ASYNCRENDERLOOP_H
