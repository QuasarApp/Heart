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

class ShedullerestNode: public QH::AbstractNode {
public:
    int executedTime = 0;

};

class TestTask: public QH::AbstractTask {


    // AbstractTask interface
public:
    bool execute(QH::AbstractNode *node) const override {

        static_cast<ShedullerestNode*>(node)->executedTime = ::time(0);

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

    int ct = time(0);
    node->sheduleTask(task);
    QVERIFY(wait([&node](){return node->executedTime;}, WAIT_TIME));
    QVERIFY(node->executedTime == (ct + 2));
    node->executedTime = 0;

    QVERIFY(node->sheduledTaskCount() == 0);

    node->softDelete();
}

void ShedullerTest::testRepeatMode() {
    ShedullerestNode *node = new ShedullerestNode();
    auto task = QSharedPointer<TestTask>::create();

    task->setMode(QH::SheduleMode::Repeat);

    int ct = time(0);
    node->sheduleTask(task);
    QVERIFY(wait([&node](){return node->executedTime;}, WAIT_TIME));
    QVERIFY(node->executedTime = (ct + 2));
    node->executedTime = 0;

    QVERIFY(node->sheduledTaskCount() == 1);

    QVERIFY(wait([&node](){return node->executedTime;}, WAIT_TIME));
    QVERIFY(node->executedTime == (ct + 4));
    QVERIFY(node->sheduledTaskCount() == 1);

    node->removeTask(task->taskId());
    QVERIFY(node->sheduledTaskCount() == 0);

    node->softDelete();

}

void ShedullerTest::testTimePointMode() {

    ShedullerestNode *node = new ShedullerestNode();
    auto task = QSharedPointer<TestTask>::create();

    task->setMode(QH::SheduleMode::TimePoint);
    int requestTime = time(0) + 2;
    task->setTime(requestTime);

    node->sheduleTask(task);
    QVERIFY(wait([&node](){return node->executedTime;}, WAIT_TIME));

    QVERIFY(node->executedTime == requestTime);
    QVERIFY(node->sheduledTaskCount() == 0);

    node->softDelete();

}
