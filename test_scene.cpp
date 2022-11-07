//
// Created by Иван Ильин on 23.02.2022.
//
#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <map>

double sum (double a, double b){
    return a+ b;
}

void printSum (double a, double b){
    std::cout << "a + b = " << a+b << std::endl;
}

class Player {
    private: 
        std::string _name;
        void secretFunction(){
            std::cout << "secretFunction from " << _name << std::endl;
        }
    public:
        Player(std::string name = "Gleb") : _name(std::move(name)){
            //_name = name;
            std::cout << "A new player " << _name << " connected to the game!" << std::endl;
        }
        virtual void jump() {
            std::cout << "jump from " << _name << std::endl;
            secretFunction();
        }
        virtual void run() const {
            std::cout << "run from " << _name << std::endl;
        }
        void setName(std::string& newName) {
            _name = std::move(newName);
        }
        [[nodiscard]]std::string getName() const {
            return _name;
        }
        Player operator + (const Player& player) {
            return Player(_name + player._name);
        }
        Player operator * (int num) {
            return Player(_name + " * " + std::to_string(num));
        } 
};

class Solder final : public Player{
    public:
    void jump() override{
        //сначала выпить воды
    }
    void shot() const{}
    void reloadWeapon() const{}
};

class Doctor final : public Player{
    public:
    void run() const override {
        //сначала встать на гироскутер
    }
    void hill() const{}
};



int main() {

    std::cout << "Hello, World!" << std::endl;

    std::cout << "myVariabes:" << std::endl;
        int a = 10;
        int b =12;
        float c = (float)a / (float)b;
    double result1 = sum(1.2, 4.5);
        double result2 = sum(1.5, 4.7);
        std::cout << "result1 = "   << result1 << std::endl;
        std::cout << "result2 = "   << result2 << std::endl;
        printSum(4.333, 5222);

    //Стандартные контейнеры С++ для управления наборами объектов

    //массив (размер контейнера(длина массива) - фиксирован) можно обращаться по индексу.
    //в даленейшем будем использовать для реализации векторов и матриц.
    std::cout << "myArray:" << std::endl;
        std::array<int, 4> myArray;
        myArray[0] = 1;
        myArray[1] =11;
        myArray[2] = 4;
        myArray[3] = 123;
        for(auto &e : myArray)
        {
            std::cout << e << std::endl;
        }

    //коллекция (размер контейнера может увеличиваться. (длинна массива - изменяемая)) можно добавлять в конец и обращаться по индексу.
    //используем для хранения данных, т.к. он удобен и не имеет ограничения на размер.
    std::cout << "myVector:" << std::endl;
        std::vector<int> myVector;
        myVector.push_back(121);
        myVector.push_back(13);
        myVector.push_back(44);
        myVector[0] = 15;
        for(auto v : myVector){
            std::cout << v << std::endl;
        }

    //двухсвязный список (структура данных построенная на двухсвязных списках) каждый элемент имеет ссылку на предыдущий и следующий.
    //мы используем для реализации алгоритма клиппинга.
    std::cout << "myList:" << std::endl;
        std::list<int> myList;
        myList.push_back(333);
        myList.push_back(222);
        myList.push_back(111);

        for(auto l : myList){
            std::cout << l << std::endl;
        }

    //словарь.
    //ассоциативный контейнер который работает по принципу "ключ" -> "значение"
    //обращаемся по ключу.
    //будем использовать для хранения объектов на карте.
    std::cout << "myMap:" << std::endl;
        std::map<std::string, int> myMap;
        myMap.insert({"car",12});
        myMap.insert({"ice",15});
        myMap.insert({"bon",18});
        myMap.insert({"tac",22});
        std::cout << "myMap[car] = " << myMap["car"] << std::endl;
        std::cout << "myMap[ice] = " << myMap["ice"] << std::endl;
        std::cout << "myMap[bon] = " << myMap["bon"] << std::endl;
        std::cout << "myMap[tac] = " << myMap["tac"] << std::endl;

    //создаем экземпляры плееров.
    Player Player1("Ivan");
    Player Player2;
    Player1.jump();
    Player2.run();

    //переопределение операторов
    Player Player3 = Player1 + Player2;
    Player Player4 = Player1 * 5;

    // Download SFML: https://www.sfml-dev.org/download.php

    sf::RenderWindow window(sf::VideoMode(1280, 720), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
