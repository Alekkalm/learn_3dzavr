//
// Created by Иван Ильин on 06.09.2022.
//

#include "Server.h"

//передаем все где сейчас находятся кубы
void Server::broadcast() {
    // TODO: implemented (lesson 10)

    sf::Packet updatePacket;
    updatePacket << MsgType::ServerUpdate; //данные с сервера

    //в наш пакет broadcast от сервера собирираем координаты всех player-ов
    for(const auto&[playerId, player] : _players){ //словарь: id и объект Player
        updatePacket << playerId << player->position().x() << player->position().y() << player->position().z();
    }

    //рассылаем пакет всем игрокам
    for(const auto&[playerId, player] : _players){
        _socket.send(updatePacket, playerId);
    }
}

//какой то новый клиент пытается подключиться к серверу и сервер должен как-то это событие обработать:
//для нового играка - нужно отправить информацию о том где находятся все старые игроки 
//всем игрокам нужно отправить информацию о том что было подключен новый клиент.
void Server::processConnect(sf::Uint16 senderId) { //senderId - Id нового клиента.
    // TODO: implemented (lesson 10)

    sf::Packet packetToNewClient;
    sf::Packet packetToOldClients;

    packetToNewClient << MsgType::Init << senderId; //для нового клиента: отправим ему его же Id который он приобрел
    packetToNewClient << MsgType::NewClient << senderId; //всем старым клиентам: отправим Id нового клиента
    
    //добавляем нового плеера в коллекцию игроков на сервере. (создаем нового Player-а).
    _players.insert({senderId, std::make_shared<Object>(ObjectNameTag("Player_" + std::to_string(senderId)))});

    //используем цикл сразу для двух дел: собираем пакет для нового игрока, и рассылаем пакеты старым игрокам.
    for(const auto&[playerId, player] : _players){
        //собираем пакент для нового клиента
        packetToNewClient << playerId << player->position().x() << player->position().y() << player->position().z();

        //рассылаем пакеты старым клиентам
        if(playerId != senderId) {
            _socket.sendRely(packetToOldClients, playerId);
        }
    }
    //новому клиенту отправляем собранный для него пакет
    _socket.sendRely(packetToNewClient, senderId);
}

//что должен сделать сервер, когда клиент обновил свои координаты:
//нужно их перезаписать:
//передвигаем плеера(в нашей коллекции плееров) в указанную точку.
void Server::processClientUpdate(sf::Uint16 senderId, sf::Packet &packet) {
    // TODO: implemented (lesson 10)

    double x, y, z;
    packet >> x >> y >> z;

    _players.at(senderId)->translateToPoint(Vec3D(x, y, z));
}

//что должен делать сервер, когда один из игроков отключается от сервера:
//нужно отправить всем игрокам сообщение что игрок был отключен, и его нужно удалить с карты.
void Server::processDisconnect(sf::Uint16 senderId) {
    // TODO: implemented (lesson 10)

    sf::Packet deleteClient;

    deleteClient << MsgType::Disconnect << senderId;
    _players.erase(senderId);

        //рассылаем пакет всем игрокам
    for(const auto&[playerId, player] : _players){
        _socket.sendRely(deleteClient, playerId);
    }
}
