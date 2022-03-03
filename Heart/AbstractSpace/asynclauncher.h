/*
 * Copyright (C) 2021-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#ifndef ASYNCLAUNCHER_H
#define ASYNCLAUNCHER_H

#include <async.h>

class QThread;

namespace QH {

/**
 * @brief The AsyncLauncher class is wraper of the Async class for support moving invokes to thread of the current object.
 * @note For create this object use the AsyncFacrtory class.
 * Exmaple:
 * @code{cpp}
 *  QThread *thread = new QThread();
 *  AsyncLauncher *async = AsyncFacrtory::makeAsyncObject<AsyncLauncher>(thread);
 *  AsyncLauncher::Job action = []() -> bool {
 *      ...
 *  }
 *
 *  async->run(action);
 * @endcode
 */
class HEARTSHARED_EXPORT AsyncLauncher final: public Async
{
    Q_OBJECT
public:
    /**
     * @brief AsyncLauncher This is base constructor of the AsyncLauncher object.
     * @param thread This is pointer to the work thread. If you want to change thread in run time just use the setThread method.
     * @param ptr This is QObject parent.
     */
    AsyncLauncher(QThread *thread, QObject* ptr = nullptr);

    /**
     * @brief run This method run the @a action function in the work thread of this object.
     * @param action This is function for running in the work thread.
     * @param wait This option for sets awaiting of the finishing the @a action function. By default this is false.
     * @return true if the action started succesful when @a wait is false and return true if the @a action finished succesful if the @a wait is true.
     */
    bool run(const Job &action, bool wait = false);

};

}
#endif // ASYNCLAUNCHER_H
