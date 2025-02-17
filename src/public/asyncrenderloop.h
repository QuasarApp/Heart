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
 * @brief The AsyncRenderLoop is a class for asynchronous rendering.
 * This class is used to create a render loop that is executed in a separate thread.
 * To use this class, you must inherit from it and implement the renderIteration method.
 * **example:** :
 * @code{cpp}
 * class MyRenderLoop: public AsyncRenderLoop
 * {
 * public:
 *    MyRenderLoop(QThread* thread, QObject* ptr = nullptr): AsyncRenderLoop(thread, ptr) {}
 *    void renderIteration(int msec) override {
 *    // your code here
 *    }
 * };
 * int main (int argc, char* argv[]) {
 *    QCoreApplication app(argc, argv);
 *
 *    MyRenderLoop loop(new thread());
 *    loop.run();
 *    loop.stop();
 *
 *    return app.exec();
 * @endcode
 */
class HEARTSHARED_EXPORT AsyncRenderLoop: public Async
{
    Q_OBJECT
public:
    AsyncRenderLoop(QThread* thread, QObject* ptr = nullptr);
    ~AsyncRenderLoop();

    /**
     * @brief run This method starts the render loop.
     */
    virtual void run();

    /**
     * @brief stop This method stops the render loop.
     */
    void stop();

    /**
     * @brief isRun This method returns the state of the render loop.
     * @return true if the render loop is running, else false.
     */
    bool isRun() const;

protected:

    /**
     * @brief renderIteration This method is called in each iteration of the render loop.
     * This method must be implemented in the derived class.
     * @param mmsec time in microseconds from the last iteration.
     * @see stop
     * @see run
     */
    virtual void renderIteration(int mmsec) = 0;

private slots:
    void renderLoopPrivate();

private:
    bool m_run = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastIterationTime;

};
}
#endif // ASYNCRENDERLOOP_H
