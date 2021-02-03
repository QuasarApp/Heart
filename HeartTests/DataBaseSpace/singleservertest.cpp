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
    QVERIFY(connectNetworkTest());
}

bool SingleServerTest::connectNetworkTest() {

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

    auto sigupRequest = [client, &user, &userPassword]() {
        return client->signup(user, userPassword);
    };

    auto checkLoginedStatus = [client](){
        return client->getStatus() == QH::ClientStatus::Logined;
    };

    auto checkConnectedStatus = [client](){
        return client->getStatus() == QH::ClientStatus::Connected;
    };

    auto checkLoginNonExitsUser = [client](){
        return client->getLastError() == static_cast<int>(QH::UserOperationResult::UserNotExits);
    };

    auto checksigupExitsUser = [client](){
        return client->getLastError() == static_cast<int>(QH::UserOperationResult::UserExits);
    };

    auto checkLoginInvalidPassword = [client](){
        return client->getLastError() == static_cast<int>(QH::UserOperationResult::UserInvalidPasswoed);
    };

    auto checkRemoveNotLoginningClient = [client](){
        return client->getLastError() == static_cast<int>(QH::UserOperationResult::UserNotLogged);
    };

    if (!(client && server)) {
        return false;
    }

    // Client must be gat a QH::ClientStatus::Dissconnected status.
    if (client->getStatus() != QH::ClientStatus::Dissconnected)
        return false;

    if (!client->connectToServer())
        return false;

    // Client must be gat a QH::ClientStatus::Connecting status after connect.
    if (client->getStatus() != QH::ClientStatus::Connecting)
        return false;

    // All attempts of the loginor or registered must be failed because the client have an offline status.
    if (client->login(user))
        return false;

    if (client->login(user, userPassword))
        return false;

    if (client->signup(user, userPassword))
        return false;

    // Run server
    if (!server->run(TEST_LOCAL_HOST, TEST_PORT, "SingleServer"))
        return false;

    // Clent must be connected because the server alredy started successful.
    if (!funcPrivateConnect([](){return true;}, checkConnectedStatus)) {
        return false;
    }

    // The attempt of the login must be failed bacause the server do not have a user with wroted name.
    if (!client->login(user))
        return false;

    // Client must be gat a QH::ClientStatus::Loginning status after a succesful login.
    if (client->getStatus() != QH::ClientStatus::Loginning)
        return false;

    if (!funcPrivateConnect([](){return true;}, checkLoginNonExitsUser)) {
        return false;
    }

    if (!funcPrivateConnect(loginRequestWithPassword, checkLoginNonExitsUser)) {
        return false;
    }

    // The attempt of the signup must be finished successful bacause user with wroted name not exists.
    if (!funcPrivateConnect(sigupRequest, checkLoginedStatus)) {
        return false;
    }

    // the login method must be return false because client alredy logginned.
    if (client->login(user, userPassword))
        return false;

    // logout client and try login again.
    client->logout();

    // This login must be failed bacause clients token is removed after logout mehod.
    if (client->login(user))
        return false;

    // must be finished succesful because client is unlogined.
    if (!funcPrivateConnect(loginRequestWithPassword, checkLoginedStatus)) {
        return false;
    }

    // disconnect client and try login again with login without password.
    client->disconnectFromServer();

    // Client must be gat a QH::ClientStatus::Dissconnected status.
    if (client->getStatus() != QH::ClientStatus::Dissconnected)
        return false;

    // Clent must be connected because the server alredy started successful.
    if (!funcPrivateConnect(connectToserver, checkConnectedStatus)) {
        return false;
    }

    // must be finished succesful because an unlogined client have a valid token.
    if (!funcPrivateConnect(loginRequestUsesToken, checkLoginedStatus)) {
        return false;
    }

    // logout client and try login again.
    client->logout();

    // must be finished failed because client wrote invalid password.
    if (!funcPrivateConnect(loginRequestWithInvalidPassword, checkLoginInvalidPassword)) {
        return false;
    }

    // check client status after invalid login. status must be equal Connected because client receive a error message from server.
    if (client->getStatus() != QH::ClientStatus::Connected)
        return false;

    // must be finished failed because client with wroted name is exists.
    if (!funcPrivateConnect(sigupRequest, checksigupExitsUser)) {
        return false;
    }

    //must be failed
    if (client->removeUser())
        return false;

    // force set status loginned for testing validation on server.
    client->setStatus(QH::ClientStatus::Logined);

    // must be finished failed because client is not loginned. and failed
    if (!funcPrivateConnect(removeRequest, checkRemoveNotLoginningClient)) {
        return false;
    }
    client->setStatus(QH::ClientStatus::Connected);


    // must be finished succesful because client is unlogined.
    if (!funcPrivateConnect(loginRequestWithPassword, checkLoginedStatus)) {
        return false;
    }

    //must be finished succesful bacause client is loginned
    if (!client->removeUser())
        return false;

    // must be finished succesful because old user is removeed.
    if (!funcPrivateConnect(sigupRequest, checkLoginedStatus)) {
        return false;
    }

    // all tests is completed
    return true;
}
