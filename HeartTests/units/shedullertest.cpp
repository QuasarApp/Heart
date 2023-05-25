//#
//# Copyright (C) 2021-2023 QuasarApp.
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


    // AbstractNode interface
public:
    NodeType nodeType() const override {
        return NodeType::Node;
    };
};

class TestTask: public QH::AbstractTask {


    // AbstractTask interface
public:
    bool execute(QH::AbstractNode *node) const override {

        static_cast<ShedullerestNode*>(node)->executedTime = QDateTime::currentMSecsSinceEpoch();

        return true;
    };

};

#ifdef Q_OS_WIN
const int stepTime = 5000;
const int measurementError = 2000;
#else
const int stepTime = 2000;
const int measurementError = 1000;
#endif

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

    task->setMode(QH::ScheduleMode::SingleWork);
    task->setTime(stepTime);

    quint64 ct = QDateTime::currentMSecsSinceEpoch();
    node->sheduleTask(task);
    QVERIFY(wait([&node](){return node->executedTime;}, WAIT_TIME));
    int diff = std::abs(static_cast<long long>(node->executedTime - (ct + stepTime)));
    QVERIFY(diff < measurementError);
    node->executedTime = 0;

    QVERIFY(node->sheduledTaskCount() == 0);

    node->softDelete();
}

void ShedullerTest::testRepeatMode() {
    ShedullerestNode *node = new ShedullerestNode();
    auto task = QSharedPointer<TestTask>::create();
    task->setTime(stepTime);
    task->setMode(QH::ScheduleMode::Repeat);

    quint64 ct = QDateTime::currentMSecsSinceEpoch();
    node->sheduleTask(task);
    QVERIFY(wait([&node](){return node->executedTime;}, WAIT_TIME));

    int diff = std::abs(static_cast<long long>(node->executedTime - (ct + stepTime)));

    QVERIFY(diff < measurementError);
    node->executedTime = 0;

    QVERIFY(node->sheduledTaskCount() == 1);

    QVERIFY(wait([&node](){return node->executedTime;}, WAIT_TIME));
    diff = std::abs(static_cast<long long>(node->executedTime - (ct + stepTime * 2)));

    QVERIFY(diff < measurementError);
    QVERIFY(node->sheduledTaskCount() == 1);

    node->removeTask(task->taskId());
    QVERIFY(node->sheduledTaskCount() == 0);

    node->softDelete();

}

void ShedullerTest::testTimePointMode() {

    ShedullerestNode *node = new ShedullerestNode();
    auto task = QSharedPointer<TestTask>::create();

    task->setMode(QH::ScheduleMode::TimePoint);

    quint64 requestTime = QDateTime::currentMSecsSinceEpoch() + 5000;
    task->setTime(requestTime);

    node->sheduleTask(task);
    QVERIFY(wait([&node](){return node->executedTime;}, WAIT_TIME));
    int diff = std::abs(static_cast<long long>(node->executedTime - requestTime));

    QVERIFY(diff < measurementError);
    QVERIFY(node->sheduledTaskCount() == 0);

    node->softDelete();

}
