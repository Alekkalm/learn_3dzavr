//
// Created by Neirokan on 30.04.2020
//

#include "MsgType.h"

sf::Packet &operator<<(sf::Packet &packet, MsgType type) {
    return packet << (sf::Uint16) type;
}

sf::Packet &operator>>(sf::Packet &packet, MsgType &type) {
    sf::Uint16 temp;
    packet >> temp;
    type = (MsgType) temp;
    return packet;
}

const char* MsgType_ToString(MsgType msgType)
{
    switch (msgType)
    {
        case MsgType::Empty:   return "Empty";
        case MsgType::Error:   return "Error";
        case MsgType::Confirm: return "Confirm";
        case MsgType::Connect:   return "Connect";
        case MsgType::Disconnect:   return "Disconnect";
        case MsgType::Init: return "Init";
        case MsgType::ServerUpdate:   return "ServerUpdate";
        case MsgType::ClientUpdate:   return "ClientUpdate";
        case MsgType::NewClient: return "NewClient";
        case MsgType::Custom:   return "Custom";
        default:      return "[Unknown MsgType]";
    }
}
