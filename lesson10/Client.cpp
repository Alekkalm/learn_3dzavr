//
// Created by Иван Ильин on 06.09.2022.
//

#include "Client.h"

//клиент отправляет серверу свое местоположение
void Client::updatePacket() {
    // TODO: implemented (lesson 10)

    sf::Packet updatePacket;
    updatePacket << MsgType::ClientUpdate << _player->position().x() << _player->position().y() << _player->position().z();
    _socket.send(updatePacket, _socket.serverId());
}

//сервер прислал положение всех игроков, нужно добавить их на карту (spawn)
void Client::processInit(sf::Packet &packet) {
    // TODO: implemented (lesson 10)

    sf::Uint16 targetId;
    double x, y, z;

    while (packet >> targetId >> x >> y >> z){ //получаем координаты
        if(targetId != _socket.ownId()){ //это вроде лишнее, т.к. мы на сервере не отправляем его, но пока делаю как на видео в уроке.
            if(_spawnPlayerCallBack != nullptr){
                _spawnPlayerCallBack(targetId); //создаем игрока.. Наконец: это видимо и есть событие. Точнее в нашем случае - просто запускаем на выполнение делегат.
            }                                   //здесь уже создался новый игрок и на карте он появился.

            //осталось сместить этого игрока в ту точку, которую мы прочитали
            _players[targetId]->translateToPoint(Vec3D(x, y, z));
        }
    }
}

//это происходит каждый раз когда сервер рассылает всем игрокам положение всех. 
void Client::processUpdate(sf::Packet &packet) {
    // TODO: implemented (lesson 10)

    sf::Uint16 id;
    double x, y, z;

    while (packet >> id >> x >> y >> z){ //получаем координаты
         if(_players.count(id)){ //если такой плеер есть,
            _players[id]->translateToPoint(Vec3D(x, y, z));//перемещаем игрока
         }
        
    }
}

//это когда сервер отправляет всем что появился новый клиент, и его нужно добавить на карту
//присылается только id этого клиента
void Client::processNewClient(sf::Packet &packet) {
    // TODO: implemented (lesson 10)

    sf::Uint16 id;
    packet >> id;
    if(_spawnPlayerCallBack != nullptr){
        _spawnPlayerCallBack(id); //создаем событие. В нашем случае - просто вызываем делегат
    }
}

//когда происходит disconnect какого-то игрока, нужно просто удалить его с карты
void Client::processDisconnect(sf::Uint16 targetId) {
    // TODO: implemented (lesson 10)
    
    if(_players.count(targetId) && targetId != _socket.ownId()){
        _players.erase(targetId); //удаляем его из коллекции у клиента.
        if(_removePlayerCallBack != nullptr){
            _removePlayerCallBack(targetId);//создаем событие. В нашем случае - просто вызываем делегат
        }
    }
}

//добавляет Player-а в коллекцию плееров которая находится в Client.
void Client::addPlayer(sf::Uint16 id, std::shared_ptr<RigidBody> player) {
    // TODO: implemented (lesson 10)
    _players.insert({id, player});
}

//добавлять игрока может только самый главный класс, поэтому здесь толко делегат
//который будет выполняться когда Engine захочет добавить игрока.
//что происходит когда игрока нужно заспавнить
void Client::setSpawnPlayerCallBack(std::function<void(sf::Uint16)> spawn) {
    _spawnPlayerCallBack = std::move(spawn);
}

//удалять игрока может только самый главный класс, поэтому здесь толко делегат
//который будет выполняться когда Engine захочет удалить игрока.
//когда игрок отключился, его нужно удалить с карты.
void Client::setRemovePlayerCallBack(std::function<void(sf::Uint16)> remove) {
    _removePlayerCallBack = std::move(remove);
}