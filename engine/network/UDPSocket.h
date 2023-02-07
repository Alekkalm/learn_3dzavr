//
// Created by Neirokan on 30.04.2020
//

#ifndef ENGINE_UDPSOCKET_H
#define ENGINE_UDPSOCKET_H

#include <memory>
#include <map>
#include <functional>

#include "ReliableMsg.h"
#include "UDPConnection.h"
#include "MsgType.h"

//все подключения будут идти через один сокет.
//данный класс хранит в себе этот один сокет.
//данный класс заведует всеми подключениями, и передает им один единственный сокет - как объект, через который подключения посылают пакеты.
//подключения используются как структуры данных - где указано куда нужно отпаравлять (и откуда получать).
//так же данный класс хранит в себе все важные пакеты которые нужно отправить. (_relyPackets)
class UDPSocket final {
private:
    sf::UdpSocket _socket;
    sf::Uint16 _nextRelyMsgId;
    sf::Uint16 _ownId;//АК: id клиента
    const sf::Uint16 _serverId = 0; //АК: id сервера

    std::map<sf::Uint16, UDPConnection> _connections;//здесь лежат id ip и порт каждого подключения (клиента)
    std::map<sf::Uint16, ReliableMsg> _relyPackets;//массив важных сообщений
    std::map<sf::Uint32, double> _confirmTimes;
    std::function<bool(sf::Uint16)> _timeoutCallback; //это событие будет вызывать методы TimeOut у Client и у Server

    bool confirmed(sf::Uint16 msgId, sf::Uint16 senderId);

public:
    explicit UDPSocket(); //конструктор

    bool bind(sf::Uint16 port);//присоединение к порту

    void unbind();

    void setTimeoutCallback(std::function<bool(sf::Uint16)> callback);//подключаем к событию функцию для выполнения

    void addConnection(sf::Uint16 id, sf::IpAddress ip, sf::Uint16 port);

    void removeConnection(sf::Uint16 id);

    void setId(sf::Uint16 id);

    [[nodiscard]] sf::Uint16 ownId() const; //собственный Id - это Id который выдал сервер нам как клиенту. 

    [[nodiscard]] sf::Uint16 serverId() const;//сервер id - это id сервера в списке подключений у нас. id сервера = 0. 

    void send(const sf::Packet &packet, const sf::IpAddress &ip, sf::Uint16 port);

    void send(const sf::Packet &packet, sf::Uint16 id);

    void sendRely(const sf::Packet &packet, const sf::IpAddress &ip, sf::Uint16 port);//надежная доставка сообщений

    void sendRely(const sf::Packet &packet, sf::Uint16 id);//надежная доставка сообщений

    void update();

    MsgType receive(sf::Packet &packet, sf::Uint16 &senderId);

    ~UDPSocket(); //деструктор
};


#endif //INC_3DZAVR_UDPSOCKET_H
