//
// Created by Иван Ильин on 06.09.2022.
//

#include "Client.h"
#include "../engine/utils/Log.h"

//клиент отправляет серверу свое местоположение
void Client::updatePacket() {
    // TODO: implemented (lesson 10)

    sf::Packet updatePacket;
    double x, y, z, ax, ay, az, aLULx, aLULy, aLULz;
    x = _player->position().x();
    y = _player->position().y();
    z = _player->position().z();
    ax = _player->angle().x();
    ay = _player->angle().y();
    az = _player->angle().z();
    aLULx = _player->angleLeftUpLookAt().x();
    aLULy = _player->angleLeftUpLookAt().y();
    aLULz = _player->angleLeftUpLookAt().z();

    Log::log("Client::updatePacket(): x=" + std::to_string(x) + " y=" + std::to_string(y) + " z=" + std::to_string(z) +
    " ax=" + std::to_string(ax) + " ay=" + std::to_string(ay) + " az=" + std::to_string(az) + 
    " aLULx=" + std::to_string(aLULx) + " aLULy=" + std::to_string(aLULy) + " aLULz=" + std::to_string(aLULz) );

    updatePacket << MsgType::ClientUpdate << x << y << z << ax << ay << az << aLULx << aLULy << aLULz;
    _socket.send(updatePacket, _socket.serverId());
        
}

//сервер прислал положение всех игроков, нужно добавить их на карту (spawn)
//это сообщение приходит один раз, при подключении к серверу.
//сюда присылаются все игроки кроме нас.
void Client::processInit(sf::Packet &packet) {
    // TODO: implemented (lesson 10)

    sf::Uint16 targetId;//это PlayerId
    double x, y, z;

    while (packet >> targetId >> x >> y >> z){ //получаем координаты
        //Log::log("Client::processInit(): PlayerId=" + std::to_string(targetId) +
        // " x=" + std::to_string(x) + " y=" + std::to_string(y) + " z=" + std::to_string(z) );

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
    double x, y, z, ax, ay, az, aLULx, aLULy, aLULz;

    while (packet >> id >> x >> y >> z >> ax >> ay >> az >> aLULx >> aLULy >> aLULz){ //получаем координаты
    //Log::log("Client::processUpdate(): PlayerId=" + std::to_string(id) +
    //     " x=" + std::to_string(x) + " y=" + std::to_string(y) + " z=" + std::to_string(z) +
    //     " ax=" + std::to_string(ax) + " ay=" + std::to_string(ay) + " az=" + std::to_string(az) +
    //     " aLULx=" + std::to_string(aLULx) + " aLULy=" + std::to_string(aLULy) + " aLULz=" + std::to_string(aLULz) );

         if(_players.count(id)){ //если такой плеер есть,
            _players[id]->translateToPoint(Vec3D(x, y, z));//перемещаем игрока
            
            _players[id]->rotateToAngle(Vec3D(ax, ay, az));

            _players[id]->rotateLeft(aLULx - _players[id]->angleLeftUpLookAt().x());
            _players[id]->rotateUp(aLULy - _players[id]->angleLeftUpLookAt().y());
            _players[id]->rotateLookAt(aLULz - _players[id]->angleLeftUpLookAt().z());
            //_players[id]->rotateLeft(rx - _players[id]->angleLeftUpLookAt().x());
            //_players[id]->rotateUp(ry - _players[id]->angleLeftUpLookAt().y());
            //_players[id]->rotateLookAt(rz - _players[id]->angleLeftUpLookAt().z());
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