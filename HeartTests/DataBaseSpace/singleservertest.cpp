/*
 * Copyright (C) 2020-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "singleservertest.h"

#include <singleserver.h>

#include <private/testsingleserver.h>
#include <private/testsingleserverclient.h>

SingleServerTest::SingleServerTest() {
    _client = new TestSingleServerClient();
    _server = new TestSingleServer();


}

SingleServerTest::~SingleServerTest() {
    _client->softDelete();
    _server->softDelete();
}

void SingleServerTest::test() {
    QVERIFY(initTest());
    loginTest();
}

void SingleServerTest::loginTest() {

    // Init default client and server objects.
    auto client = dynamic_cast<TestSingleServerClient*>(_client);
    auto server = dynamic_cast<TestSingleServer*>(_server);

    const QString user = "client";
    const QString userPassword = "123";

    // init using lymbda functions.
    auto connectToserver = [client](){
        return client->connectToServer();
    };

    auto removeRequest = [client]() {
        return client->removeUser();
    };

    auto loginRequestWithPassword = [client, &user, &userPassword]() {
        return client->login(user, userPassword);
    };

    auto loginRequestWithInvalidPassword = [client, &user]() {
        return client->login(user, "zz");
    };

    auto loginRequestUsesToken = [client, &user]() {
        return client->login(user);
    };

    auto logout = [client]() {
        return client->logout();
    };

    auto sigupRequest = [client, &user, &userPassword]() {
        return client->signup(user, userPassword);
    };

    auto dissconnectRequest = [client]() {
        client->disconnectFromServer();
        return true;
    };

    auto checkLoginedStatus = [client](){
        return client->getStatus() == QH::ClientStatus::Logined;
    };

    auto checkDisconnectedStatus = [client](){
        return client->getStatus() == QH::ClientStatus::Dissconnected;
    };

    auto checkConnectedStatus = [client](){
        return client->getStatus() == QH::ClientStatus::Connected;
    };

    auto checkLoginNonExitsUser = [client](){
        return client->takeLastError() == static_cast<int>(QH::ErrorCodes::UserNotExits);
    };

    auto checksigupExitsUser = [client](){
        return client->takeLastError() == static_cast<int>(QH::ErrorCodes::UserExits);
    };

    auto checkLoginInvalidPassword = [client](){
        return client->takeLastError() == static_cast<int>(QH::ErrorCodes::UserInvalidPasswoed);
    };

    auto checkRemoveNotLoginningClient = [client](){
        return client->takeLastError() == static_cast<int>(QH::ErrorCodes::UserNotLogged);
    };

    QVERIFY(client && server);

    if (!client)
        return;

    // Client must be gat a QH::ClientStatus::Dissconnected status.
    QVERIFY(client->getStatus() == QH::ClientStatus::Dissconnected);

    QVERIFY(client->connectToServer());


    // Client must be gat a QH::ClientStatus::Connecting status after connect.
    QVERIFY(client->getStatus() == QH::ClientStatus::Connecting);

    // All attempts of the loginor or registered must be failed because the client have an offline status.
    QVERIFY(!client->login(user));

    QVERIFY(!client->login(user, userPassword));

    QVERIFY(!client->signup(user, userPassword));

    // Run server
    QVERIFY(server->run(TEST_LOCAL_HOST, LOCAL_TEST_PORT, "SingleServer"));

    // Clent must be connected because the server alredy started successful.
    QVERIFY(funcPrivateConnect(connectToserver, checkConnectedStatus));

    // The attempt of the login must be failed bacause the server do not have a user with wroted name.
    QVERIFY(client->login(user, userPassword));

    // Client must be gat a QH::ClientStatus::Loginning status after a successful login.
    QVERIFY(client->getStatus() == QH::ClientStatus::Loginning);

    QVERIFY(funcPrivateConnect([](){return true;}, checkLoginNonExitsUser));

    QVERIFY(funcPrivateConnect(loginRequestWithPassword, checkLoginNonExitsUser));

    // The attempt of the signup must be finished successful bacause user with wroted name not exists.
    QVERIFY(funcPrivateConnect(sigupRequest, checkLoginedStatus));

    // logout client and try login again.
    QVERIFY(funcPrivateConnect(logout, checkConnectedStatus));

    // waiting of the reflection of the server. This needed because server do not responce about successful logout.
    wait([](){return false;}, 500);

    // This login must be failed bacause clients token is removed after logout mehod.
    QVERIFY(!client->login(user));

    // must be finished successful because client is unlogined.
    QVERIFY(funcPrivateConnect(loginRequestWithPassword, checkLoginedStatus));

    // disconnect client and try login again with login without password.
    QVERIFY(funcPrivateConnect(dissconnectRequest, checkDisconnectedStatus));

    // Client must be gat a QH::ClientStatus::Dissconnected status.
    QVERIFY(client->getStatus() == QH::ClientStatus::Dissconnected);

    // Clent must be connected because the server alredy started successful.
    QVERIFY(funcPrivateConnect(connectToserver, checkConnectedStatus));

    // must be finished successful because an unlogined client have a valid token.
    QVERIFY(funcPrivateConnect(loginRequestUsesToken, checkLoginedStatus));

    // logout client and try login again.
    QVERIFY(funcPrivateConnect(logout, checkConnectedStatus));

    // waiting of the reflection of the server. This needed because server do not responce about successful logout.
    wait([](){return false;}, 500);

    // must be finished failed because client wrote invalid password.
    QVERIFY(funcPrivateConnect(loginRequestWithInvalidPassword, checkLoginInvalidPassword));

    // check client status after invalid login. status must be equal Connected because client receive a error message from server.
    QVERIFY(client->getStatus() == QH::ClientStatus::Connected);

    // must be finished failed because client with wroted name is exists.
    QVERIFY(funcPrivateConnect(sigupRequest, checksigupExitsUser));

    //must be failed
    QVERIFY(!client->removeUser());

    // force set status loginned for testing validation on server.
    client->setStatus(QH::ClientStatus::Logined);

    // must be finished failed because client is not loginned. and failed
    QVERIFY(!funcPrivateConnect(removeRequest, checkRemoveNotLoginningClient));

    client->setStatus(QH::ClientStatus::Connected);

    // must be finished successful because client is unlogined.
    QVERIFY(funcPrivateConnect(loginRequestWithPassword, checkLoginedStatus));

    //must be finished successful bacause client is loginned
    QVERIFY(client->removeUser());

    // waiting of the reflection of the server. This needed because server do not responce about successful operation.
    wait([](){return false;}, 500);

    // must be finished successful because old user is removeed.
    QVERIFY(funcPrivateConnect(sigupRequest, checkLoginedStatus));

    // all tests is completed

}

bool SingleServerTest::initTest() {
    auto server = dynamic_cast<TestSingleServer*>(_server);

    if (!server->run(TEST_LOCAL_HOST, LOCAL_TEST_PORT, "SingleServer")) {
        return false;
    }

    QString database = server->dbLocation();
    server->stop();

    if (QFileInfo::exists(database) && !QFile::remove(database)) {
        return false;
    }

    return true;
}
