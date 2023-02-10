//
// Created by Neirokan on 30.04.2020
//

#include "ClientUDP.h"
#include "MsgType.h"
#include "../utils/Time.h"
#include "../utils/Log.h"
#include "../Consts.h"

ClientUDP::ClientUDP() {
    _socket.setTimeoutCallback([this](sf::Uint16 id) { return ClientUDP::timeout(id); });
}

bool ClientUDP::connected() const {
    return _socket.ownId();
}

bool ClientUDP::isWorking() const {
    return _working;
}

void ClientUDP::connect(sf::IpAddress ip, sf::Uint16 port) {
    _ip = ip; //ip сервера на который нужно слать пакеты
    _port = port; //порт сервера на который нужно слать пакеты
    sf::Packet packet;
    packet << MsgType::Connect << Consts::NETWORK_VERSION;
    _working = _socket.bind(sf::Socket::AnyPort);//подключаемся к свободному порту для прослушивания. (0 - означает порт который выдас ОС. можно вместо 0 писать AnyPort) 
    _working ? Log::log("ClientUDP::connect(): Bound to Port " + std::to_string(_socket.GetLocalPort())) :Log::log("ClientUDP::connect(): Bound to port is failed")  ;
    _socket.addConnection(_socket.serverId(), ip, port);
    _socket.sendRely(packet, _socket.serverId());

    Log::log("ClientUDP::connect(): connecting to the server...");
}

void ClientUDP::update() {
    if (!isWorking()) {
        return;
    }

    //крутимся в бесконечном цикле, пока не получим все собщения пришедшие (пока process не вернет false).
    //  Если process вернет true - значит было получено сообщение
    while (isWorking() && process()) {}


    //после того как мы обработали все получения сообщения
    //мы отправляем пакет серверу
    // Send new client information to server
    if (Time::time() - _lastBroadcast > 1.0 / Consts::NETWORK_WORLD_UPDATE_RATE && connected()) {
        updatePacket();//отправляем неважные сообщения (текущую позицию)
        _lastBroadcast = Time::time();//последний раз мы отправляли сообщение - сейчас.
    }

    // Socket update
    _socket.update();//отправляем важные сообщения (например: подключиться)
}

void ClientUDP::disconnect() {
    sf::Packet packet;
    packet << MsgType::Disconnect << _socket.ownId();
    _socket.send(packet, _socket.serverId());//отправляем сообщение что произошел Disconnect
    _socket.unbind(); //отключаемся от сокета
    _working = false; //выставляем статус - что не работаем

    Log::log("ClientUDP::disconnect(): disconnected from the server.");
    processDisconnected(); //вызываем виртуальную функцию. (например здесь мы можем удалять всех игроков с карты)
}

bool ClientUDP::timeout(sf::Uint16 id) {
    Log::log("ClientUDP::timeout(): timeout from the server.");

    if (id != _socket.serverId()) {
        return true;
    }
    disconnect(); //если таймаут, то вызываем функцию disconnect
    return false;
}

//получение пакета
// Recive and process message.
// Returns true, if some message was received.
bool ClientUDP::process() {
    sf::Packet packet;
    sf::Uint16 senderId;//сюда возвращается id того кто прислал
    sf::Uint16 targetId; //это наш Id который нам назначит сервер когда нас подключит. (MsgType::Init)

    //считываем пакет, id отправителя, тип сообщения.
    //Если это был запрос Connect от клиента, то ему назначается свободный Id, и новый клиент добавляется в наш список игроков.
    //после считывания пакета, сюда возвращается пакет без первых 4 элементов. (без SenderId, reply, MsgId, MsgType)
    MsgType type = _socket.receive(packet, senderId);

    if (type == MsgType::Empty) {
        return false; //сообщений больше нет
    }
    if (!connected() && type != MsgType::Init) {
        return true; //получено сообщение. Говорим что получено сообщение чтобы продолжать читать новые сообщения,
    }   // но ничего не делаем, т.к. ждем сообщения Init ?

    switch (type) {
        // here we process any operations based on msg type
        case MsgType::Init:             //если инициализация
            packet >> targetId;         //значит сервер назначил нам id
            _socket.setId(targetId);    //сохраняем его

            Log::log("ClientUDP::process(): received MsgType::Init. Server send us client Id = " + std::to_string(targetId));
            Log::log("It means our status = connected");

            processInit(packet); //без первых 4 элементов. (без SenderId, reply, MsgId, MsgType)
            break;
        case MsgType::ServerUpdate:

            processUpdate(packet); //без первых 4 элементов. (без SenderId, reply, MsgId, MsgType)
            break;
        case MsgType::NewClient://мы получили нового клиента

            Log::log("ClientUDP::process(): new client init...");

            processNewClient(packet); //без первых 4 элементов. (без SenderId, reply, MsgId, MsgType)
            break;
        case MsgType::Disconnect://произошел Disconnect
            packet >> targetId; 
            if (targetId == _socket.ownId()) { //если id равен нашему id
                disconnect();                  //значит нам нужно отключиться от сервера.
            }

            Log::log("ClientUDP::process(): client Id = " + std::to_string(targetId) + " disconnected from the server");

            processDisconnect(targetId); //передаем id того кто отключается.
            break;
        case MsgType::Custom: //дополнительный пользовательский пакет
            processCustomPacket(packet);
            break;
        case MsgType::Error:
            Log::log("ClientUDP::process(): Error message");
            break;
        default: //получили вообще неизвестный пакет
            Log::log("ClientUDP::process(): unknown message type " + std::to_string(static_cast<int>(type)));
    }

    return true;
}
