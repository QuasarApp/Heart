//#
//# Copyright (C) 2021-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "shedullertest.h"

#include "abstractnode.h"
#include "abstracttask.h"
#include "ctime"
#include <QTest>
#include <QDateTime>
#include <cmath>
class ShedullerestNode: public QH::AbstractNode {
public:
    quint64 executedTime = 0;

};

class TestTask: public QH::AbstractTask {


    // AbstractTask interface
public:
    bool execute(QH::AbstractNode *node) const override {

        static_cast<ShedullerestNode*>(node)->executedTime = QDateTime::currentMSecsSinceEpoch();

        return true;
    };

};

ShedullerTest::ShedullerTest() {

}

void ShedullerTest::test() {

    testSingleMode();
    testRepeatMode();
    testTimePointMode();
}

void ShedullerTest::testSingleMode() {
    ShedullerestNode *node = new ShedullerestNode();
    auto task = QSharedPointer<TestTask>::create();

    task->setMode(QH::SheduleMode::SingleWork);
    task->setTime(2);

    quint64 ct = QDateTime::currentMSecsSinceEpoch();
    node->sheduleTask(task);
    QVERIFY(wait([&node](){return node->executedTime;}, WAIT_TIME));
    int diff = std::abs(static_cast<long long>(node->executedTime - (ct + 2000)));
    QVERIFY(diff < 1000);
    node->executedTime = 0;

    QVERIFY(node->sheduledTaskCount() == 0);

    node->softDelete();
}

void ShedullerTest::testRepeatMode() {
    ShedullerestNode *node = new ShedullerestNode();
    auto task = QSharedPointer<TestTask>::create();
    task->setTime(2);
    task->setMode(QH::SheduleMode::Repeat);

    quint64 ct = QDateTime::currentMSecsSinceEpoch();
    node->sheduleTask(task);
    QVERIFY(wait([&node](){return node->executedTime;}, WAIT_TIME));

    int diff = std::abs(static_cast<long long>(node->executedTime - (ct + 2000)));

    QVERIFY(diff < 1000);
    node->executedTime = 0;

    QVERIFY(node->sheduledTaskCount() == 1);

    QVERIFY(wait([&node](){return node->executedTime;}, WAIT_TIME));
    diff = std::abs(static_cast<long long>(node->executedTime - (ct + 4000)));

    QVERIFY(diff < 1000);
    QVERIFY(node->sheduledTaskCount() == 1);

    node->removeTask(task->taskId());
    QVERIFY(node->sheduledTaskCount() == 0);

    node->softDelete();

}

void ShedullerTest::testTimePointMode() {

    ShedullerestNode *node = new ShedullerestNode();
    auto task = QSharedPointer<TestTask>::create();

    task->setMode(QH::SheduleMode::TimePoint);

    int requestTime = time(0) + 5;
    task->setTime(requestTime);

    node->sheduleTask(task);
    QVERIFY(wait([&node](){return node->executedTime;}, WAIT_TIME));
    int diff = std::abs(static_cast<long long>(node->executedTime - requestTime * 1000));

    QVERIFY(diff < 1000);
    QVERIFY(node->sheduledTaskCount() == 0);

    node->softDelete();

}
