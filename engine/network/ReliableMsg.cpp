//
// Created by Neirokan on 30.04.2020
//

#include <limits>

#include "ReliableMsg.h"
#include "../utils/Time.h"
#include "../Consts.h"

ReliableMsg::ReliableMsg(sf::Packet &packet, sf::IpAddress address, sf::Uint16 port) : packet(packet), address(address),
                                                                                       port(port),
                                                                                       lastTry(-std::numeric_limits<double>::max()),
                                                                                       firstTry(Time::time()) {}

ReliableMsg::ReliableMsg(const ReliableMsg &msg) : packet(msg.packet), address(msg.address), port(msg.port),
                                                   lastTry(msg.lastTry), firstTry(msg.firstTry) {}

bool ReliableMsg::trySend(sf::UdpSocket &socket) {
    if (Time::time() - firstTry > Consts::NETWORK_TIMEOUT) {//если с самой первой отправки уже прошло 5сек, то больше не посылаем
        return false;
    }
    if (Time::time() - lastTry > Consts::NETWORK_RELIABLE_RETRY_TIME) {//если с последней отправки прошло больше 50мсек, то посылаем (первый раз, или еще раз)
        lastTry = Time::time();
        socket.send(packet, address, port);
    }
    return true;
}
