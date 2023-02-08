//
// Created by Neirokan on 30.04.2020
//

#include "ServerUDP.h"
#include "MsgType.h"
#include "../utils/Log.h"

ServerUDP::ServerUDP() {
     //timeoutCallBack функция призвана удалять соединения от сервера
    _socket.setTimeoutCallback([this](sf::Uint16 playerId) { return timeout(playerId); });
}

bool ServerUDP::isWorking() const {
    return _working;
}

bool ServerUDP::start(sf::Uint16 port) { //когда мы стартуем сервер
    _working = _socket.bind(port);       //мы биндимся к порту

    if (_working) {
        Log::log("ServerUDP::start(): the server was successfully started. Bound to port Number " + std::to_string(port));
    } else {
        Log::log("ServerUDP::start(): failed to start the server.");
    }

    return _working;
}

void ServerUDP::update() {
    if (!isWorking()) {
        return;
    }

    while (process()) {} //получаем сообщения

    // World state broadcast
    if (Time::time() - _lastBroadcast > 1.0 / Consts::NETWORK_WORLD_UPDATE_RATE) {
        broadcast(); //в клиенте мы делаем в этом месте update, а здесь мы делаем broadcast (отправляем всем клиентам какую либо информацию)
        _lastBroadcast = Time::time();
    }

    // Socket update
    _socket.update();

    updateInfo();
}

//когда мы вызываем функцию Stop:
void ServerUDP::stop() {
    for (auto it = _clients.begin(); it != _clients.end();) {
        sf::Packet packet;
        packet << MsgType::Disconnect << *it; //посылаем всем клиентам что они отсоединены от сервера
        _socket.send(packet, *it);
        _clients.erase(it++);
    }

    _socket.unbind();
    _working = false;

    processStop();

    Log::log("ServerUDP::stop(): the server was killed.");
}

//сервер вызывает функцию Таймаут - когда от playerId давно небыло никаких сообщений.
bool ServerUDP::timeout(sf::Uint16 playerId) {
    sf::Packet packet;
    packet << MsgType::Disconnect << playerId; //всем клиентам отправляем сообщение что этого игрока нужно дисконектить.

    _clients.erase(playerId); //и удаляем этого клиента

    for (auto client : _clients) {
        _socket.sendRely(packet, client);
    }

    Log::log("ServerUDP::timeout(): client Id = " + std::to_string(playerId) + " disconnected due to timeout.");
    processDisconnect(playerId);

    return true;
}

// Recive and process message.
// Returns true, if some message was received.
bool ServerUDP::process() {
    sf::Packet packet;
    sf::Packet sendPacket;
    sf::Uint16 senderId;

    MsgType type = _socket.receive(packet, senderId);//считываем пакет и id отправителя

    if (type == MsgType::Empty) {
        return false;
    }

    //и дальше по типу сообщения смотрит как отвечать и как реагировать.
    switch (type) {
        // here we process any operations based on msg type
        case MsgType::Connect:  //если произошло соединение
            Log::log("ServerUDP::process(): client Id = " + std::to_string(senderId) + " connecting...");
            //здесь по видео должно быть _clients.insert(senderId); видимо произошли улучнения какие-то
            processConnect(senderId);
            break;
        case MsgType::ClientUpdate:

            processClientUpdate(senderId, packet);
            break;
        case MsgType::Disconnect: //когда произошел дисконнект:
            Log::log("ServerUDP::process(): client Id = " + std::to_string(senderId) + " disconnected.");

            sendPacket << MsgType::Disconnect << senderId;  //отправляем всем клиентам что произоше дисконнект
            _clients.erase(senderId);       //удаляем из клиентов senderId
            _socket.removeConnection(senderId); //удаляем его Connection
            for (auto client : _clients) {
                _socket.sendRely(sendPacket, client);
            }

            processDisconnect(senderId);
            break;
        case MsgType::Custom:
            processCustomPacket(packet, senderId);
            break;
        case MsgType::Error:
            Log::log("ServerUDP::process(): Error message");
            break;
        default:
            Log::log("ServerUDP::process(): message type " + std::to_string(static_cast<int>(type)));
    }

    return true;
}

ServerUDP::~ServerUDP() {
    stop();
    _clients.clear();
}
