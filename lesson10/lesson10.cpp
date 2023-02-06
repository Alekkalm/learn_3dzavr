//
// Created by Иван Ильин on 06.09.2022.
//

#include "../engine/Engine.h"
#include "../engine/utils/ObjectController.h"

#include "Server.h"
#include "Client.h"

class Lesson10 : public Engine {
private:
    std::shared_ptr<RigidBody> player = world->loadBody(ObjectNameTag("Player"), "obj/cube.obj");
    ObjectController playerController;

    std::shared_ptr<Server> server = std::make_shared<Server>();
    std::shared_ptr<Client> client = std::make_shared<Client>(player);

    bool is_camera_controlling = false;

    void initNetwork() {
        // TODO: implemented (lesson 10)

        //здесь мы должны:
        //подключить сервер, подключить клиент
        //выбрать ip, выбрать порт, и т.д.


        std::string clientIp = "127.0.0.1"; //если clientIp совпадает с localHost - это значит что мы должны еще и запустить сервер
        sf::Uint16 clientPort = 54000;
        sf::Uint16 serverPort = 54000;

        if(clientIp == sf::IpAddress::LocalHost){ //если два приложения запустить на одном же комптьютере с   clientIp = "127.0.0.1"
            server->start(serverPort);            //то, т.к. первый сервер уже порт занял, то второй сервер не сможет запуститься.
        }

        //подписываемся на событие клиента.
        //клиент будет будет дергать событие, и будет выполнятся функция spawnPlayer(id).
        client->setSpawnPlayerCallBack([this](sf::Uint16 id){spawnPlayer(id); });
        client->setRemovePlayerCallBack([this](sf::Uint16 id){removePlayer(id); });

        client->connect(clientIp, clientPort);
    }

public:
    Lesson10() : playerController(player, keyboard, mouse) {}; //конструктор

    void start() override {

        player->translate(Vec3D(0,0,5));

        setGlEnable(true);
        initNetwork();

        while (client->isWorking() && !client->connected()) { //пока клиент работает, и не подключен к серверу, крутимся здесь, ждем подключения
            client->update();
            server->update();
            Time::update();
        }
    }

    void update() override { //апдейт - делаем после того как вышли из цикла "подключение"

        server->update(); //если сервер не запускали, то он не будет обновлятся.
        client->update();//сам игрок подключается к самому себе же (если у него белый IP)

        // Check all input after this condition please
        if (!screen->hasFocus()) {
            return;
        }

        if(keyboard->isKeyPressed(sf::Keyboard::Escape)) { //не вышли ли мы из игры
            exit();
        }

        if(keyboard->isKeyTapped(sf::Keyboard::C)) { //нажали ли мы на кнопку "С" или нет. Если нажали то перестаем контролировать камеру.
            is_camera_controlling = !is_camera_controlling;
        }

        if(is_camera_controlling) { //если камеру нужно контролировать, то мы обновляем ПлейерКонтроллер
            playerController.update();
        }
    }

    
    void spawnPlayer(sf::Uint16 id) { //появление (рождение) игрока
        // TODO: implement (lesson 10)

        //что у нас происходит когда спавнится новый игрок:
        //1. нужно задать ему имя
        std::string name = "Player_" + std::to_string(id);

        //создадим объект которому присвоим это имя
        auto newPlayer = std::make_shared<RigidBody>(ObjectNameTag(name), "obj/cube.obj");
        //добавим его на карту
        world->addBody(newPlayer);
        //и добавим его же в коллекцию к клиенту:
        client->addPlayer(id, newPlayer);
    }


    void removePlayer(sf::Uint16 id) { //удаление игрока
        // TODO: implemented (lesson 10)

        //мы должны обратится к миру и передать ему имя
        world->removeBody(ObjectNameTag("Player_" + std::to_string(id)));
    }
};

int main() {
    /*
     * EN:
     * TODO: implement simple multiplayer
     * You should work with the following files:
     *
     * lesson10/Server.cpp
     * lesson10/Client.cpp
     * lesson10/lesson10.cpp
     *
     * If you did everything correctly, then when you start two or more clients, cubes
     * should appear on the screen and changes in the coordinates of one of the cubes
     * should be displayed on all connected clients
     *
     * RU:
     * TODO: реализуйте простой мультиплеер
     * На этом уроке вам необходимо работать со следующими файлами:
     *
     * lesson10/Server.cpp
     * lesson10/Client.cpp
     * lesson10/lesson10.cpp
     *
     * Если вы всё сделали правильно, то при запуске двух и более клиентов на экране должны
     * появятся кубы и изменения координат одного из кубов должно отображаться на всех
     * подключённых клиентах
     */

    Lesson10 l;
    l.create(1280, 720);
}
