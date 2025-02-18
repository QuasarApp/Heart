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
 *
 * @warning This class may be broken if you use it as a QSharedPointer and push WeackPointer to the child objects. To solve this issue use the @a AsyncRenderLoop::MainSharedPtr class.
 *
 * Example:
 *
 * @code{cpp}
 * class MyRenderLoop: public AsyncRenderLoop
 * {
 * ...
 * };
 *
 * int main (int argc, char* argv[]) {
 *   auto loop = QSharedPointer<MyRenderLoop>(new MyRenderLoop(new QThread())); // wrong! it will be broken
 *   auto loop = MyRenderLoop::MainSharedPtr<QSharedPointer<MyRenderLoop>>(QSharedPointer<MyRenderLoop>::create(new QThread())); // right!
 *  ...
 *  return app.exec();
 *  }
 *  @endcode
 */
class HEARTSHARED_EXPORT AsyncRenderLoop: public Async
{

    Q_OBJECT
public:

    /**
     * @brief The MainSharedPtr class is a helper class for creating a shared pointer to the render loop.
     * This class make main sharedPointer of your render loop object. it is used to solve issue with deleting object in self thread.
     *
     * if you use the AsyncRenderLoop as a QSharedPointer and push WeackPointer to the child objects, you must use this wrapper class.
     */
    template<typename T>
    class MainSharedPtr {
    public:
        MainSharedPtr(const T& ptr): _ptr(ptr) {
            static_assert(std::is_base_of_v<AsyncRenderLoop, typename T::element_type>,
                          "T must be derived from QSharedPointer<AsyncRender>");
        }
        ~MainSharedPtr() {
            if (_ptr) {
                _ptr->stop();
            }
        }

        typename T::element_type* operator->() const {
            return _ptr.operator->();
        }

        typename T::element_type* get() const {
            return _ptr.get();
        }

    private:
        T _ptr;
    };

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
