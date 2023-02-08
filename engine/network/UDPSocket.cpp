//
// Created by Neirokan on 30.04.2020
//

#include <algorithm>

#include "UDPSocket.h"
#include "../utils/Time.h"
#include "../Consts.h"
#include "../utils/Log.h"

UDPSocket::UDPSocket() : _ownId(0), _nextRelyMsgId(0) {
    _socket.setBlocking(false);
}

void UDPSocket::addConnection(sf::Uint16 id, sf::IpAddress ip, sf::Uint16 port) {
    _connections.insert({id, UDPConnection(id, ip, port)});
}

void UDPSocket::removeConnection(sf::Uint16 id) {
    _connections.erase(id);
}

bool UDPSocket::bind(sf::Uint16 port) { //присоединение к порту. Возвращаем статус: удалось или нет.
    Log::log("UDPSocket::bind(): bind to port Number: " + std::to_string(port));
    return _socket.bind(port) == sf::Socket::Status::Done;
}

void UDPSocket::unbind() { //команда отключиться от порта. рассылаем всем подключениям сообщение что мы Disconnect
    sf::Packet packet;
    packet << MsgType::Disconnect << _ownId;

    for (auto it = _connections.begin(); it != _connections.end();) {
        send(packet, it->first);
        _connections.erase(it++);
    }

    _relyPackets.clear();
    _confirmTimes.clear();
    _socket.unbind();
    setId(0);
}

void UDPSocket::setTimeoutCallback(std::function<bool(sf::Uint16)> callback) {
    _timeoutCallback = std::move(callback);
}

void UDPSocket::setId(sf::Uint16 id) {
    _ownId = id;
}

sf::Uint16 UDPSocket::ownId() const {
    return _ownId;
}

sf::Uint16 UDPSocket::serverId() const {
    return _serverId;
}

//надежная(с ожиданием подтверждения) отправка по ip и port
void UDPSocket::sendRely(const sf::Packet &packet, const sf::IpAddress &ip, sf::Uint16 port) {
    sf::Packet finalPacket;
    //формируем заголовок:
    // "id-отправителя"    "нужно отвечать"  "id-этого важного сообщения"
    finalPacket << _ownId << true << _nextRelyMsgId; //_nextRelyMsgId - локальная переменная, нигде больше не используется кроме как здесь.
    finalPacket.append(packet.getData(), packet.getDataSize());
    _relyPackets.insert({_nextRelyMsgId++, ReliableMsg(finalPacket, ip, port)}); //после отправки?, _nextRelyMsgId инкриментируется
}       //но похоже сообщение не отправляется, а складывается в коллекцию важных пакетов.

//надежная(с ожиданием подтверждения) отправка по id
void UDPSocket::sendRely(const sf::Packet &packet, sf::Uint16 id) {
    if (!_connections.count(id)) {
        return;
    }
    this->sendRely(packet, _connections.at(id).ip(), _connections.at(id).port());
}

//отправка по ip и port
void UDPSocket::send(const sf::Packet &packet, const sf::IpAddress &ip, sf::Uint16 port) {
    sf::Packet finalPacket;
    //формируем заголовок:
    // "id-отправителя"    "ненужно отвечать"  "id-куда нужно отправить"
    finalPacket << _ownId << false << _serverId;
    finalPacket.append(packet.getData(), packet.getDataSize());
    _socket.send(finalPacket, ip, port);
}

//отправка по id
void UDPSocket::send(const sf::Packet &packet, sf::Uint16 id) {
    if (!_connections.count(id)) {
        return;
    }
    this->send(packet, _connections.at(id).ip(), _connections.at(id).port());
}

//отправляем важные сообщения,
//проверяем Таймауты.
void UDPSocket::update() { //обновляем все коннекшены
    for (auto it = _connections.begin(); it != _connections.end();) {
        if (!it->second.timeout()) { //если Тайаут НЕ произошел, то мы инкрементируемся и идем дальше.
            ++it;
        } else { //Таймаут произошел.
            if (_timeoutCallback && !_timeoutCallback(it->first)) { //если задана функция CallBack то мы её вызываем, передаем туда id подключения.
                return;                                             //если функция _timeoutCallback(it->first) вернула true, значит это был сервер
            }                                                       //т.е. сервер давно не отвечает, и можно уже не обрабатывать остальные подключения

            _connections.erase(it++);                               //иначе - это обрыв от клиента, удаляем это подключение
        }
    }

    //отправляем все важные сообщения
    for (auto it = _relyPackets.begin(); it != _relyPackets.end();) {
        if (!it->second.trySend(_socket)) { //отправляем. если отправить не удалось, 
            _relyPackets.erase(it++);       //то мы его удаляем
        } else {
            ++it;                           //иначе - идем к следующему сообщению.
        }
    }

    //времена подтверждения важных сообщений
    //(они хранятся чтобы увидеть что подтверждение не дошло до адресата.)
    //если подтверждения были давно, то срок хранения можно считать законченым, и удаляем эти данные.
    for (auto it = _confirmTimes.begin(); it != _confirmTimes.end();) {
        if (Time::time() - it->second > Consts::NETWORK_TIMEOUT) {
            _confirmTimes.erase(it++);
        } else {
            ++it;
        }
    }
}

//прием сообщений.
//возвращает тип сообщения который он получил.
//если ничего не получил, то MsgType::Empty
MsgType UDPSocket::receive(sf::Packet &packet, sf::Uint16 &senderId) {
    // Receive message
    sf::IpAddress ip;
    sf::Uint16 port;

    packet.clear();
    if (_socket.receive(packet, ip, port) != sf::Socket::Status::Done) {
        return MsgType::Empty;//если сообщение небыло получено, то возвращает пустое сообщение
    }
    //в противном случае мы идем дальше

    // Read header (расшифровываем наш заголовок)
    bool reply = false;
    sf::Uint16 msgId = 0;
    MsgType type;
    senderId = 0;
    if (!(packet >> senderId >> reply >> msgId >> type)) {
        return MsgType::Error;
    }

    //теперь мы знаем кто именно отправил нам сообщение, 
    //и в соединении с этим id делаем update.
    if (_connections.count(senderId)) {
        _connections.at(senderId).update(); //т.е. обновить время последнего получения сообщения
    }

    //дальше мы смотрим какой именно тип сообщения мы получили
    if (type == MsgType::Confirm) {//если это подтверждение о получении важного сообщения, то мы стираем сообщение из списка важных
        _relyPackets.erase(msgId);
        // you don't need this information on the highest levels
        return MsgType::Empty;
    }

    if (type == MsgType::Connect) {//подключение нового клиента
        sf::Uint32 version = 0; //версия - это какая версия нашей реализации протокола 
        if (!(packet >> version) || version != Consts::NETWORK_VERSION) { //не удалось получить версию, или если наша версия и версия полученная из пакета не совпадает
            return MsgType::Error;//просто возвращаем сообщение об ошибке. ни как не интерпретируем и не пытаемся обработать
        }

        //выбираем id для нового пользователя
        sf::Uint16 tmp;
        //цикл for выполняется 64 раза. Ищется минимальный свободный id, который запоминается в переменную sender_Id.
        //проверяем все занятые id, что у них не такой же ip и порт. 
        for (tmp = Consts::NETWORK_MAX_CLIENTS; tmp >= 1; tmp--) { //перебираем все подряд от максимального до 0 (можно было сделать и наоборот?)
            if (!_connections.count(tmp)) { //count(k) - возвращает количество элементов с ключем k. Если подключения с таким id нет,
                senderId = tmp;             //то выбираем это id
            } else if (_connections.at(tmp).same(ip, port)) { //иначе если этот id уже занят, проверяем, не такой же у него ip и port
                return MsgType::Error;                        //чтобы небыло несколько подключений с одинаковыми ip и портом.
            }                                                 //т.е. вдруг клиент шлет несколько раз запрос на подключение - добавляем его толко один раз
        }

        //создаем новое подключение и добавляем его в наш словарь подключений.
        _connections.insert({senderId, UDPConnection(senderId, ip, port)});
    }

    //проверяем что все нормально сработало
    if (!_connections.count(senderId) ||            //что подключение с новым id есть в списке подключений
     !_connections.at(senderId).same(ip, port) ||   //что у подключения с новым id правильныей ip и port
        reply && confirmed(msgId, senderId)) {      //если нужно ответить, отправляем ответ и проверяем "это уже повторное подтверждение?" 
        return MsgType::Error;                   //если одно из условий сработало, то возвращаем ошибку.
    }
    return type;
}

//функция подтверждения
bool UDPSocket::confirmed(sf::Uint16 msgId, sf::Uint16 senderId) {
    sf::Packet confirmPacket;
    //формируем пакет
    // "свой id"  "на это сообщение уже отвечать ненужно" "id сообщения которое подтверждаем"  "говорим что это тип - подтверждение"
    confirmPacket << _ownId << false << msgId << MsgType::Confirm;
    _connections.at(senderId).send(_socket, confirmPacket); //отправляем

    //создаем уникальный id состоящий из id отправителя и id сообщения
    //соединяем два 16-битных числа в одно 32-битное
    sf::Uint32 confirmId;
    confirmId = (senderId << 16) | msgId;//сдвигаем на 16 бит и делаем "побитное или" 

    bool repeat = _confirmTimes.count(confirmId);//подтверждали ли мы это сообщение раньше (есть ли такой у нас в сохраненных)
    _confirmTimes[confirmId] = Time::time();//запоминаем время когда мы отправили сообщение о получении важного сообщения

    return repeat;
}

UDPSocket::~UDPSocket() {
    unbind();
}
