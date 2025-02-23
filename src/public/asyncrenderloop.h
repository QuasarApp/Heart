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
 *   auto loop = MyRenderLoop::MainSharedPtr<MyRenderLoop>>(new QThread()); // right!
 *   auto loop = MyRenderLoop::createMainPtr<MyRenderLoop>(new QThread()); // this is short version of initialization Main pointer
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
     * @tparam T type of the render loop object.
     * This class make main sharedPointer of your render loop object. it is used to solve issue with deleting object in self thread.
     *
     * if you use the AsyncRenderLoop as a QSharedPointer and push WeackPointer to the child objects, you must use this wrapper class.
     */
    template<typename T>
    class MainSharedPtr {
    public:
        MainSharedPtr() {
            static_assert(std::is_base_of_v<AsyncRenderLoop, T>,
                          "T must be derived from AsyncRenderLoop");
        }

        MainSharedPtr(const QSharedPointer<T>& ptr): _ptr(ptr) {
            static_assert(std::is_base_of_v<AsyncRenderLoop, T>,
                          "T must be derived from AsyncRenderLoop");
        }
        ~MainSharedPtr() {
            if (_ptr) {
                _ptr->stop();
            }
        }

        T* operator->() const {
            return _ptr.operator->();
        }

        /**
         * @brief get This is a alias of the QSharedPointer::get method.
         * @return pointer to the object.
         */
        T* get() const {
            return _ptr.get();
        }

        /**
         * @brief create This method creates a shared pointer to the render loop.
         * @param arguments arguments for the constructor of the render loop object.
         * @return shared pointer to the render loop.
         */
        template <typename... Args>
        [[nodiscard]] static MainSharedPtr create(Args && ...arguments) {
            return MainSharedPtr(QSharedPointer<T>::create(std::forward<Args>(arguments)...));
        }

        /**
         * @brief getShared This method return child shared pointer. You can use them as a general shared pointer of the object.
         * @return reference to the object.
         */
        const QSharedPointer<T>& getShared() const {
            return _ptr;
        }

    private:
        QSharedPointer<T> _ptr;
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

    /**
     * @brief createMainPtr This method creates a shared pointer to the render loop.
     * @tparam Type type of the render loop object.
     * @tparam Args arguments for the constructor of the render loop object.
     * @param arguments arguments for the constructor of the render loop object.
     * @return shared pointer to the render loop.
     */
    template<typename Type, typename... Args>
    static MainSharedPtr<Type> createMainPtr(Args && ...arguments) {
        return MainSharedPtr<Type>(QSharedPointer<Type>::create(std::forward<Args>(arguments)...));
    };

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
