//
// Created by Иван Ильин on 05.02.2021.
//

#include <cmath>
#include <utility>
#include <limits>

#include "RigidBody.h"
#include "../utils/Log.h"
#include "../utils/Time.h"
#include "../Consts.h"

RigidBody::RigidBody(ObjectNameTag nameTag, const std::string &filename, const Vec3D &scale, bool useSimpleBox) : Mesh(std::move(nameTag),
                                                                                                    filename, scale),
                                                                                               _hitBox(*this, useSimpleBox) {
}

RigidBody::RigidBody(const Mesh &mesh, bool useSimpleBox) : Mesh(mesh), _hitBox(mesh, useSimpleBox) {
}

void RigidBody::updatePhysicsState() {
    // TODO: implemented (lesson 5)
    _velocity = _velocity + _acceleration * Time::deltaTime();
    translate(_velocity * Time::deltaTime());
}

void RigidBody::setVelocity(const Vec3D &velocity) {
    // TODO: implemented (lesson 5)
    _velocity =velocity;
}

void RigidBody::addVelocity(const Vec3D &velocity) {
    // TODO: implemented (lesson 5)
    _velocity = _velocity + velocity;
}

void RigidBody::setAcceleration(const Vec3D &acceleration) {
    // TODO: implemented (lesson 5)
    _acceleration = acceleration;
}

//поиск самой дальней точки из всех точек hitBox-а в заданном в direction направлении 
Vec3D RigidBody::_findFurthestPoint(const Vec3D &direction) {
    Vec3D maxPoint{0, 0, 0};
    double maxDistance = -std::numeric_limits<double>::max();

    //чтобы не переводить каждую точку в мировую систему координат, 
    //наоборот переведем вектор направления в систему координат нашего объекта:
    Vec3D transformedDirection = (invModel()*direction).normalized();

    //_hitBox - это облако уникальных точек для модели. (т.к. в треугольнках точки повторяются(так в нашей программе сделано))
    //т.е. куб - это 6 граней. каждая грать - это 2 треугольника. каждый треугольник состоит из 3 точек. получаем 6*2*3=36точек.
    //но у куба - 8 уникальных точек.
    for(auto & it : _hitBox) {
        // TODO: implemented (lesson 6)

       
        //находим расстояние до каждой точки. 
        double distance = it.dot(transformedDirection);


        //переведем точку в мировую систему координат (но без смещения - только поворот, т.к. этого достаточно чтобы найти крайнюю точку)
        //находим расстояние до каждой точки. 
        //(это точно работает, но мы будем использовать transformedDirection чтобы уменьшить вычисления)
        //double distance = (model()*it).dot(direction.normalized());


        if(distance > maxDistance){
            //если расстояние (проекция на вектор направления) больше чем у нас было сохранено, то запоминаем эти значения:
            maxDistance = distance;
            maxPoint = it;
        }
    }
    //найденную точку переводим в мировую систему координат:
    //умножаем на матрицу модели (которая в себе содержит уже смещение,
    //но т.к. в hitBox точки не 4D а 3D, то смещения не произойдет,
    //поэтому мы смещение добавляем дополнително еще раз.)
    return model()*maxPoint + position();
}

//функция находит крайние точки (опорные точки) разности Миньковского (для заданного направления).
//имеются в виду вершины коллаидеров.
//если для одной фигуры ищется максимальная проекция на вектор направления,
//то для второй фигуры ищется максимальная проекция в противоположное направление.
Vec3D RigidBody::_support(std::shared_ptr<RigidBody> obj, const Vec3D &direction) {
    // TODO: implemented (lesson 6)
    Vec3D p1 = _findFurthestPoint(direction);
    Vec3D p2 = obj->_findFurthestPoint(-direction);
    //возвращаем точку разности Миньковского:
    return p1 - p2;
}

//новый симплекс:
//возвращает новое направление для поиска.
//может удалить одну дальнюю точку, если она не подошла.
//если обноружено что начало координат внутри тетраидера, то сообщает что поиск можно прекращать.
NextSimplex RigidBody::_nextSimplex(const Simplex &points) {
    switch (points.type()) {
        case SimplexType::Line:
            return _lineCase(points);
        case SimplexType::Triangle:
            return _triangleCase(points);
        case SimplexType::Tetrahedron:
            return _tetrahedronCase(points);

        default:
            throw std::logic_error{"RigidBody::_nextSimplex: simplex is not Line, Triangle or Tetrahedron"};
    }
}

//новый симплекс, вариант если пришла линия:
NextSimplex RigidBody::_lineCase(const Simplex &points) {
    Simplex newPoints(points);
    Vec3D newDirection;

    //если пришла линия, значит нам передали 2 точки,
    //создадим временные переменные:
    Vec3D a = points[0];//точка а - это последняя найденная (новая) точка. (т.к. найденные точки добавляются спереди)
    Vec3D b = points[1];

    Vec3D ab = b - a;//вектор из точки а в точку b.
    Vec3D ao = -a; //вектор который смотрит из точки a в направлении о (начала координат)

    // TODO: implemented (lesson 6)
    //если точка 0,0,0 находится где-то между точками а и b, то
    if(ab.dot(ao) > 0){
        //новое направление поиска - перпендикулярно отрезку аb в направлении о (начала координат)
        //сначала находим вектор который смотрин перпендикулярно плоскости: ab.cross(ao),
        //а потом - в направлении о (начала координат): ..cross(ab);
        newDirection = ab.cross(ao).cross(ab);

        //if(newDirection == Vec3D(0,0,0)){
        //    return NextSimplex{newPoints, newDirection, true};
        //}

    }else{
        //иначе (вектор ab смотрит не в сторону начала координат)
        //точку b можно удалить, и поискать точку в направлении начала координат.
        newPoints = Simplex{a};
        newDirection = ao;
    }

    return NextSimplex{newPoints, newDirection, false};
}

//новый симплекс, вариант если пришел треугольник (три точки):
NextSimplex RigidBody::_triangleCase(const Simplex &points) {
    Simplex newPoints(points);
    Vec3D newDirection;

    //если пришел треугольник, значит нам передали 3 точки,
    //создадим временные переменные:
    Vec3D a = points[0];//точка а - это последняя найденная (новая) точка. (т.к. найденные точки добавляются спереди)
    Vec3D b = points[1];
    Vec3D c = points[2];

    Vec3D ab = b - a;//вектор из точки а в точку b.
    Vec3D ac = c - a;//вектор из точки а в точку c.
    Vec3D ao = -a;//вектор который смотрит из точки a в направлении о (начала координат)

    // TODO: implemented (lesson 6)

    //вспомогательный вектор перпендикулярный треугольнику abc
    Vec3D abc = ab.cross(ac);
    //если начало координат находится за стороной ac:
    if(abc.cross(ac).dot(ao) > 0){
        //если начало координат находится в зоне перпендикуляра к отрезку ac
        if(ac.dot(ao) > 0){
            //мы оставляем отрезок ac (удаляем точку b)
            //и задаем направление поиска в сторону начала координат
            newPoints = Simplex{a, c};
            newDirection = ac.cross(ao).cross(ac);
        }else{
            //в противном случае - мы возвращаемся к случаю линии со стороной ab
            return _lineCase(Simplex{a, b});
        }
    }else{
        //иначе остались еще два варианта:
        //начало координат находится за прямой ab
        if(ab.cross(abc).dot(ao) > 0){
            //переходим к случаю линии ab
            return _lineCase(Simplex{a, b});
        }else{
            //начало координат находится внутри треугольника
            //нам нужно задать новое направление поиска
            //перпендикулярно треугольнику (чтобы повторно не найти уже найденные точки).
            //если вектор abc (он перпендекулярен треугольнику abc) и вектор ao смотрят в одну сторону относительно плоскости треугольника
            if(abc.dot(ao) > 0){
                //то новое направление поиска - в сторону вектора abc
                newDirection = abc;
            }else{
                //в противном случае - новое направление - противоположно вектору abc
                newDirection = -abc;
                //точки переставляем местами, чтобы их нормаль была в направлении поиска. (как и в первом случае)
                newPoints = Simplex{a, c, b};
            }

        }

    }

    return NextSimplex{newPoints, newDirection, false};
}

//трехмерных симплекс (тетраэдр)
NextSimplex RigidBody::_tetrahedronCase(const Simplex &points) {
    Vec3D a = points[0];//точка а - это последняя найденная (новая) точка. (т.к. найденные точки добавляются спереди)
    Vec3D b = points[1];
    Vec3D c = points[2];
    Vec3D d = points[3];

    Vec3D ab = b - a;//вектор из точки а в точку b.
    Vec3D ac = c - a;//вектор из точки а в точку с.
    Vec3D ad = d - a;//вектор из точки а в точку d.
    Vec3D ao = -a;//вектор который смотрит из точки a в направлении о (начала координат)

    // TODO: implemented (lesson 6)
    //т.к. точка а - это последняя добавленная точка, то
    //bcd - это основание пирамиды.
    //поэтому мы будем исследовать боковые грани (все они содержат точку а).

    //найдем векторы нормали к боковым граням (векторы которые смотрят наружу тетраэдра)
    //так же мы знаем что вектор bcd (bc.cross(bd)) смотрит в сторону точки а.(смотреть вектор abc для _triangleCase)
    Vec3D abc = ab.cross(ac);
    Vec3D acd = ac.cross(ad);
    Vec3D adb = ad.cross(ab);

    //если начало координат лежит со стороны нормали боковой поверхности (снаружи тертаэдра)
    //то удаляем точку не принадлежащей этой боковой поверхности и возвращаемся к случаю треугольника
    if(abc.dot(ao) > 0){
        return _triangleCase(Simplex{a, b, c});
    }
    if(acd.dot(ao) > 0){
        return _triangleCase(Simplex{a, c, d});
    }
    if(adb.dot(ao) > 0){
        return _triangleCase(Simplex{a, d, b});
    }

    //если не выпали предыдущие три варианта, значит начало координат находится внутри тертаэдера.
    //выставляем признак для остановки поиска, новое направление можно не задавать.
    return NextSimplex{points, Vec3D(), true};
}

std::pair<bool, Simplex> RigidBody::checkGJKCollision(std::shared_ptr<RigidBody> obj) {
    // This is implementation of GJK algorithm for collision detection.
    // It builds a simplex (a simplest shape that can select point in space) around
    // zero for Minkowski Difference. Collision happened when zero point is inside.

    // Get initial support point in any direction
    //для первого направления берем направление по оси Х:
    Vec3D support = _support(obj, Vec3D{1, 0, 0});

    // Simplex is an array of points, max count is 4
    Simplex points{};
    points.push_front(support);

    // New direction is towards the origin
    //следующее направление - направление в сторону начала координат.
    //в нашем случае - это противоположное направление - направление -Х:
    Vec3D direction = -support;

    //количество итераций не должно быть больше чем 
    //количество точек коллаидера одного объекта + второго объекта.
    size_t iters = 0;
    while (iters++ < size() + obj->size()) {
        // TODO: implemented (lesson 6)
        support = _support(obj, direction);//находим следующую точку
        //если найденная точка находится в противоположном направлении от направления поиска,
        //то никакого столкновения нет
        if(support.dot(direction) <= 0){
            return std::make_pair(false, points);//no collision
        }
        //иначе - добавляем точку в массив Points
        points.push_front(support);

        //сторим новый симплекс, и передаем ему все точки которые успели собрать
        NextSimplex nextSimplex = _nextSimplex(points);
        //проверяем что вернет нам симплекс:
        //новый симплекс - нам либо удалит точку либо не удалит, и вернет новое направление для поиска.
        //либо выдаст признак что начало координат найдено внутри тертаэдера, т.е. обнаружено столкновение.
        direction = nextSimplex.newDirection;
        points = nextSimplex.newSimplex;
        //если мы закончили искать (nextSimplex сказал что столкновение обнаружено), то выходим:
        if(nextSimplex.finishSearching){
            //если объект являлся колаидером, то пометим что столкновение обнаружено, (и его нужно разрешать(обработать)):
            if(obj->isCollider()){
                 _inCollision = true;
            }
            //возвращаем признак что столкновение обнаружено, и возвращаем найденый симплекс.
            //(этот симплекс (тетраэдр) - является частью разности Миньковского от наших двух фигур,
            // внутри которого мы нашли начало координат)
            return std::make_pair(true, points);//collision
        }
    }
    //если мы вышли из цикла(закончили все итерации), но так и незакончили поиск 
    //то будем считать что нет никакого столкновения.
    return std::make_pair(false, points);
}

CollisionInfo RigidBody::EPA(const Simplex &simplex, std::shared_ptr<RigidBody> obj) {
    // This is implementation of EPA algorithm for solving collision.
    // It uses a simplex from GJK around and expand it to the border.
    // The goal is to calculate the nearest normal and the intersection depth.

    //создаем коллекцию точек (из коллекции точек симплекса) которую мы назовем polytop - т.е. многогранник.
    //(видимо есть такой конструкотор у коллекции, когда ей передают начальный и конечный итератор другой коллекции)
    std::vector<Vec3D> polytope(simplex.begin(), simplex.end());
    //создаем коллекцию целых чисел - которые будут являться гранями нашего политопа
    //(size_t - базовый беззнаковый целочисленнй тип, обычно применяется для счетчиков цикла и индексации массива)
    //тут мне пока непонятно, как мы узнали правильное расположение точек..
    //возможно что если 0,1,2 - это поверхность которая смотрит наружу, то точка 3 находится с обратной стороны нормали,
    //и на основе этих знаний мы записали расположение всех граней.
    //** оказывается дальше, в функции _getFaceNormals, мы проверяем направление нормалей, и исправляем, а расположение точек не меняем.
    std::vector<size_t> faces = {
            0, 1, 2,
            0, 3, 1,
            0, 2, 3,
            1, 3, 2
    };

    auto faceNormals = _getFaceNormals(polytope, faces); //получаем коллекцию нормалей, и индекс ближайшей к центру
    std::vector<FaceNormal> normals = faceNormals.first; //запоминаем коллекцию нормалей в локальную переменную
    size_t minFace = faceNormals.second; //запоминаем индекс ближайшей к центру нормали

    Vec3D minNormal = normals[minFace].normal;//получаем вектор ближайшей нормали. 
    double minDistance = std::numeric_limits<double>::max();

    size_t iters = 0;
    //пока не найдем хоть какойнибудь minDistance, или не пройдемся по всем точкам по 3 раза
    while (minDistance == std::numeric_limits<double>::max() && iters++ < size() + obj->size()) {
        // TODO: implemented (lesson 7)
        minNormal = normals[minFace].normal;
        minDistance = normals[minFace].distance;

        //ищем новую точку в направлении нормали
        Vec3D support = _support(obj, minNormal);
        //проверяем: действительно ли новая точка новая, а не та которая уже была в нашем симплексе
        double sDistance = minNormal.dot(support); //расстояние до поверхности по нормали.
        
        //если разница между этими расстояниями оказалось больше числа эпсилон, то
        //мы производим добавление новой точки
        //в противном случае:
        //мы не обновляем minDistance, а так как она уже не равна бесконечности, то цикл While закончится
        if(std::abs(sDistance - minDistance) > Consts::EPA_EPS){
             minDistance = std::numeric_limits<double>::max();//делаем minDistance = бесконечности, чтобы цикл While не закончился 

             //теперь будем добавлять грани.
             //1. нужно определить какие стороны граней являются уникальными
             std::vector<std::pair<size_t, size_t>> uniqueEdges;//уникальные стороны (т.е. ребра)

             size_t f = 0; //переменная f для того чтобы сохранять сдвиг

             for(auto& normal : normals){ //проходим по всем нормалям
                //если новая точка находится в направлении нормали,
                //то для этой грани нужно будет тоже находить уникальные стороны
                if(normal.normal.dot(support) > 0) {
                    //передаем в функцию - коллекцию уникальных сторон, коллекцию граней, индексы точек - сторону, которую мы хотим добавить.
                    uniqueEdges = _addIfUniqueEdge(uniqueEdges, faces, f + 0, f + 1);
                    uniqueEdges = _addIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
                    uniqueEdges = _addIfUniqueEdge(uniqueEdges, faces, f + 2, f + 0);

                    //удаляем грань (т.е. точки грани), которая учавствует при добавлении новой вершины
                    faces.erase(faces.begin() + f);
                    faces.erase(faces.begin() + f);
                    faces.erase(faces.begin() + f);
                }else{ //если нормаль грани находится не в направлении новой точки, то
                        //просто сдвигаем индекс f на 3:
                        f +=3;
                }
            }
            //У нас есть уникальные стороны, и все грани которые учавствовали в формировании уникальных сторон были удалены
            //теперь нужно добавить новые грани в наш массив
            std::vector<size_t> newFaces;
            newFaces.reserve(uniqueEdges.size()*3);//резервируем размер массива (определяется количеством уникальных сторон)

            //для всех индексов уникальных граней мы создаем отдельно от старых - новые грани (newFaces) :
            for(auto[i1, i2] : uniqueEdges){
                //добавляем две наши вершины, а третьей вершиной является новая найденная вершина.








                //проверяем чтобы нормаль смотрела наружу
                Vec3D a = polytope[faces[i1]];
                Vec3D b = polytope[faces[i2]];
                Vec3D c = support;

                //создаем вектор нормали для треугольника
                Vec3D normal = (b - a).cross(c - a).normalized();

                //у всех точек плоскости проекция на вектор нормали одинаковая и равна расстоянию от начала координат до плоскости.
                //поэтому берем любую точку, например а: 
                double distance = normal.dot(a);

                //если расстояние оказалось отрицательным, значит вектор нормали смотрит в сторну начала координат, (т.е. внутрь фигуры)
                //поэтому меняем ему знак, чтобы он смотрел наружу
                if (distance < -Consts::EPS) {
                        
                    #include <iostream>
                    #include <Windows.h> // Обязательно для SetConsoleCP() и SetConsoleOutputCP()
                    SetConsoleCP(65001);
                    SetConsoleOutputCP(65001);
                    
                    std::cout << "меняем направление нормали" << std::endl;

                    newFaces.push_back(i2);
                    newFaces.push_back(i1);
                }else{
                    newFaces.push_back(i1);
                    newFaces.push_back(i2);
                }

                newFaces.push_back(polytope.size());//точку в политоп мы еще не добавили, но индекс на нее все равно можно уже указать
            }

            //добавляем нашу новую точку в политоп:
            //(расширяем наш симплекс)
            polytope.push_back(support);

            //ко всем старым граням нужно добавить новые грани
            faces.insert(faces.end(), newFaces.begin(), newFaces.end());

            //осталось посчитать новые вектора нормалей для всех граней 
            //и вычислить минимальные расстояния:
            auto newFaceNormals = _getFaceNormals(polytope, faces);
            normals = std::move(newFaceNormals.first);
            minFace = newFaceNormals.second;
        }
    }

    _collisionNormal = minNormal;
    if (std::abs(minDistance - std::numeric_limits<double>::max()) < Consts::EPS) {
        return CollisionInfo{minNormal, 0};
    }

    return CollisionInfo{minNormal, minDistance + Consts::EPA_EPS, obj};
}

std::pair<std::vector<FaceNormal>, size_t>
RigidBody::_getFaceNormals(const std::vector<Vec3D> &polytope, const std::vector<size_t> &faces) {
    //объявляем временные переменные:
    std::vector<FaceNormal> normals; //коллекция нормалей (будут в той же последовательности что и грани(поверхности) политопа)
    normals.reserve(faces.size() / 3); //размер коллекции нормалей (т.к. faces - это номера вершин каждого треугольника, то номера вершин повторяются в соседних треугольника, поэтому их в 3 раза больше чем треугольников)
    size_t nearestFaceIndex = 0;
    double minDistance = std::numeric_limits<double>::max();

    //перебираем все треугольники
    for (size_t i = 0; i < faces.size(); i += 3) {
        Vec3D a = polytope[faces[i + 0]];
        Vec3D b = polytope[faces[i + 1]];
        Vec3D c = polytope[faces[i + 2]];

        //создаем вектор нормали для треугольника
        Vec3D normal = (b - a).cross(c - a).normalized();

        //у всех точек плоскости проекция на вектор нормали одинаковая и равна расстоянию от начала координат до плоскости.
        //поэтому берем любую точку, например а: 
        double distance = normal.dot(a);

        //если расстояние оказалось отрицательным, значит вектор нормали смотрит в сторну начала координат, (т.е. внутрь фигуры)
        //поэтому меняем ему знак, чтобы он смотрел наружу
        if (distance < -Consts::EPS) {
            normal = -normal;
            distance *= -1;
            	
            #include <iostream>
            #include <Windows.h> // Обязательно для SetConsoleCP() и SetConsoleOutputCP()
            SetConsoleCP(65001);
            SetConsoleOutputCP(65001);
            
            std::cout << "инверсия нормали: i= " << i << std::endl;//"\n";

            //попробуем поменять местами вершины
            //size_t b = faces[i + 1];
            //size_t c = faces[i + 2];
            //faces[i + 1] = c;
            //faces[i + 2] = b;

        }

        //складываем в конец, чтобы первый остался первым.
        normals.emplace_back(FaceNormal{normal, distance});

        //находим минимальное расстояние
        if (distance < minDistance) {
            nearestFaceIndex = i / 3;
            minDistance = distance;
        }
    }

    //возвращаем все найденные нормали (там внутри вектор нормали и расстояние до начала координат), и индекс самого ближайшего к центру
    return {normals, nearestFaceIndex};
}

//метод добавляет уникальные стороны
std::vector<std::pair<size_t, size_t>>
RigidBody::_addIfUniqueEdge(const std::vector<std::pair<size_t, size_t>> &edges, const std::vector<size_t> &faces,
                            size_t a, size_t b) {

    std::vector<std::pair<size_t, size_t>> newEdges = edges;

    // We are interested in reversed edge
    //      0--<--3
    //     / \ B /   A: 2-0
    //    / A \ /    B: 0-2
    //   1-->--2

    // TODO: implemented (lesson 7)
    //в эту функцию передаются только стороны, принадлежащие граням с вектором нормали в сторону новой точки.
    //дальше - непонятки, т.к. точки вроде не выстроены в правильной очередности чтобы нормали смотрели наружу..?

    //если мы будем передвигаться по соседним неуникальным сторонам, то они будут ориентированы в противоположную сторону
    //поэтому чтобы убедится что сторона аб уже содержится в массиве, нужно искать сторону ба.

    //ищем в коллеции уникальных сторон, сторону с противоположными индексами.
    auto reverse = std::find(newEdges.begin(), newEdges.end(), std::make_pair(faces[b], faces[a]));

    //если нашли
    if(reverse != newEdges.end()){
        newEdges.erase(reverse);//то стираем найденный. то есть это ребро неуникальное, и оно нам ненужно совсем.
    }else{ //если такого еще небыло
        newEdges.emplace_back(faces[a], faces[b]);//добавляем в коллекцию уникальных.
    }
    return newEdges;
}

void RigidBody::solveCollision(const CollisionInfo &collision) {
    // TODO: implemented (lesson 7)
    //нам нужно переместить первое тело относительно второго
    //CollisionInfo - это то что мы возвращали из метода EPA: вектор нормали ближайшей к центру грани, и расстояние до ближайшей грани.
    //поэтому двигаем тело в обратном направлении (в направлении к центру), на величину расстояния до ближайшей грани.
    translate(-collision.normal * collision.depth);
    //это странный виртуальный метод, который нигде не определен. 
    collisionCallBack(collision);
}
