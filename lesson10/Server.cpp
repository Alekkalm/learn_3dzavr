//
// Created by Иван Ильин on 06.09.2022.
//

#include "Server.h"
#include "../engine/utils/Log.h"

//передаем все где сейчас находятся кубы
void Server::broadcast() {
    // TODO: implemented (lesson 10)

    double x, y, z, ax, ay, az, aLULx, aLULy, aLULz;
 
    sf::Packet updatePacket;
    updatePacket << MsgType::ServerUpdate; //данные с сервера

    //в наш пакет broadcast от сервера собирираем координаты всех player-ов
    for(const auto&[playerId, player] : _players){ //словарь: id и объект Player

        x = player->position().x();
        y = player->position().y();
        z = player->position().z();
        ax = player->angle().x();
        ay = player->angle().y();
        az = player->angle().z();
        aLULx = player->angleLeftUpLookAt().x();
        aLULy = player->angleLeftUpLookAt().y();
        aLULz = player->angleLeftUpLookAt().z();

        updatePacket << playerId << x << y << z << ax << ay << az << aLULx << aLULy << aLULz;

        Log::log("Server::broadcast(): x=" + std::to_string(x) + " y=" + std::to_string(y) + " z=" + std::to_string(z) +
        " ax=" + std::to_string(ax) + " ay=" + std::to_string(ay) + " az=" + std::to_string(az) + 
        " aLULx=" + std::to_string(aLULx) + " aLULy=" + std::to_string(aLULy) + " aLULz=" + std::to_string(aLULz) );
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
    packetToOldClients << MsgType::NewClient << senderId; //всем старым клиентам: отправим Id нового клиента
    
    //добавляем нового плеера в коллекцию игроков на сервере. (создаем нового Player-а).
    _players.insert({senderId, std::make_shared<Object>(ObjectNameTag("Player_" + std::to_string(senderId)))});

    //используем цикл сразу для двух дел: собираем пакет для нового игрока, и рассылаем пакеты старым игрокам.
    for(const auto&[playerId, player] : _players){
        //собираем пакет для нового клиента
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

    double x, y, z, ax, ay, az, aLULx, aLULy, aLULz;

    packet >> x >> y >> z >> ax >> ay >> az >> aLULx >> aLULy >> aLULz;

    Log::log("Server::processClientUpdate(): x=" + std::to_string(x) + " y=" + std::to_string(y) + " z=" + std::to_string(z) +
    " ax=" + std::to_string(ax) + " ay=" + std::to_string(ay) + " az=" + std::to_string(az) + 
    " aLULx=" + std::to_string(aLULx) + " aLULy=" + std::to_string(aLULy) + " aLULz=" + std::to_string(aLULz) );

    _players.at(senderId)->translateToPoint(Vec3D(x, y, z));
    //_players.at(senderId)->rotate(
    //Vec3D(  (ax - _players[senderId]->angle().x()),
    //        (ay - _players[senderId]->angle().y()),
    //        (az - _players[senderId]->angle().z()))
    //);
     _players.at(senderId)->rotateToAngle(Vec3D(ax, ay, az));

    _players[senderId]->rotateLeft(aLULx - _players[senderId]->angleLeftUpLookAt().x());
    _players[senderId]->rotateUp(aLULy - _players[senderId]->angleLeftUpLookAt().y());
    _players[senderId]->rotateLookAt(aLULz - _players[senderId]->angleLeftUpLookAt().z());
    
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
