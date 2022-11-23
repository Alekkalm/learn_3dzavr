//
// Created by Иван Ильин on 14.01.2021.
//

#include <cmath>

#include "Camera.h"
#include "utils/Log.h"
#include "Consts.h"

std::vector<Triangle> Camera::project(std::shared_ptr<Mesh> mesh) {

    if (!_ready) {
        Log::log("Camera::project(): cannot project _tris without camera initialization ( Camera::init() ) ");
        return _triangles;
    }

    if (!mesh->isVisible()) {
        return this->_triangles;
    }

    Matrix4x4 M = mesh->model();
    Matrix4x4 V = Matrix4x4::View(left(), up(), lookAt(), position());

    for (auto &t : mesh->triangles()) {
        // TODO: implement (lessons 2, 3, 4)
        //Lesson2 - проецируем треугольники на экран.
        //АК: здесь мы треугольник модели из системы координат модели (обычно начало координат модели находится в центре модели, чтобы удобно было вращать)
        //переводим в ситему координат мира. для этого умножаем каждый треугольник модели на матрицу модели (матрицу сдвига модели).
        // t * M
        Triangle modelTiangle = t*M;

        //вставочка из урока 3 (Lesson3) 
        //закрашивание треугольников в зависимости от угла нормали треугольника к направлению на камеру:
        //если нормаль треугольника смотрит точно на камеру, то яркость максимальная.
        //если нормаль треугольника отворачивается от направления на камеру, то яркость понижается.
        //
        //скалярное произведение нормали треугольника и 
        //вектора направленного от треугольника к камере ((вектор позиции камеры) минус (вектор позиции треугольника)).
        //Оба вектора - нормализаваные, чтобы получить чистый косинус угла между ними.
        // modelTiangle.position() - это середина треугольника.
        // Vec3D(modelTiangle[0]) - это первая точка треугольника.
        //вот так - моделька cube.obj выглядит красивее. 
        //double dot = modelTiangle.norm().dot((position() - Vec3D(modelTiangle[0])).normalized());
        //мне нравится так:
        double dot = modelTiangle.norm().dot((position() - modelTiangle.position()).normalized());
        //куб выглядит похуже, но это всего лишь из за того что мы не все точки раскрашиваем в зависимости от угла к этой точке,
        //а целиком треугольник расскрашиваем.
        //поэтому вместо градиента освещености мы получаем на кубе всего два треугольника раскрашеных в два цвета.

        //если скалярное произведение меньше 0, то значит нормаль треугольника отвернута от камеры, такой треугольник мы не
        //будем показывать (не будем отрисовывать на экране)
        if(dot < 0){
            continue; //чтобы рисовались все треугольники - закоментировать.
        }
        //конец вставки из урока 3 (Lesson3).

        //затем, когда получили треугольник в мировой системе координат, 
        //умножаем на матрицу проецирования P - получаем координаты на экране
        //только единицы измерения - нормированные, т.е. половина высоты экрана = 1, и половина ширины экрана = 1.
        //затем, умножаем на матрицу  экранного пространства S,
        //которая преобразует единицы измерения из нормированных в пиксели экрана.
        //(матрицы P и S уже умножены друг на друга заранее, т.к. они являются константами, получив матрицу _SP = S*P)
        Triangle projected = modelTiangle*_SP;
        //АК: здесь "projected_normalized" - имеется ввиду что мы закладывали что после преобразования в проекцию с помощью матрицы P
        //нам надо еще будет поделить все координаты на Z, тогда мы получим нормализованную матрицу.
        //но мы еще и на матрицу S уже умножили, поэтому после деления на Z мы уже получим матрицу
        //нормализованную по оси Z (0 - 1), а по осями x и у - нормализованная (0 - 1) будет преобразована в (0 - ширина экрана) и (0 - высота экрана)
        //но на z же мы еще не поделили, поэтому делим на Z:
        // т.к. оригинальна Z(нетронутая преобразованиями) у нас хранится в w. (у временной переменной "projected")
        Triangle projected_normalized = Triangle(projected[0]/projected[0].w(),
                                                 projected[1]/projected[1].w(),
                                                 projected[2]/projected[2].w());

//но после деления (которое выше этого коментария) - в переменной w уже хранится 1 (в переменной "projected_normalized"). 

        //вставочка из урока 3 (Lesson3)
        //цвет треугольника:
        //double dotColor = 0.1 * std::abs(dot) + 0.9; //сделаем чтобы косинус влиял только на 10% всего диапазона цвета.
        double dotColor = 0.4 * std::abs(dot) + 0.6; //оставлю так. 40% от диапазона. Т.е. в самом темном месте будет яркость 60%.
        //берем цвет треугольника и умножаем его на наш коффициент:
        projected_normalized.setColor(
            sf::Color(
                t.color().r * dotColor,
                t.color().g * dotColor,
                t.color().b * dotColor
            ));
        //конец вставки из урока 3 (Lesson3).

                                                 
                                                
        
        _triangles.emplace_back(projected_normalized);
    }

    return this->_triangles;
}

void Camera::init(int width, int height, double fov, double ZNear, double ZFar) {
    // We need to init camera only after creation or changing width, height, fov, ZNear or ZFar.
    // Because here we calculate matrix that does not change during the motion of _objects or camera
    _aspect = (double) width / (double) height;
    Matrix4x4 P = Matrix4x4::Projection(fov, _aspect, ZNear, ZFar);
    Matrix4x4 S = Matrix4x4::ScreenSpace(width, height);

    _SP = S * P; // screen-space-projections matrix

    // This is planes for clipping _tris.
    // Motivation: we are not interest in _tris that we cannot see.
    _clipPlanes.emplace_back(Plane(Vec3D{0, 0, 1}, Vec3D{0, 0, ZNear})); // near plane
    _clipPlanes.emplace_back(Plane(Vec3D{0, 0, -1}, Vec3D{0, 0, ZFar})); // far plane

    double thetta1 = Consts::PI * fov * 0.5 / 180.0;
    double thetta2 = atan(_aspect * tan(thetta1));
    _clipPlanes.emplace_back(Plane(Vec3D{-cos(thetta2), 0, sin(thetta2)}, Vec3D{0, 0, 0})); // left plane
    _clipPlanes.emplace_back(Plane(Vec3D{cos(thetta2), 0, sin(thetta2)}, Vec3D{0, 0, 0})); // right plane
    _clipPlanes.emplace_back(Plane(Vec3D{0, cos(thetta1), sin(thetta1)}, Vec3D{0, 0, 0})); // down plane
    _clipPlanes.emplace_back(Plane(Vec3D{0, -cos(thetta1), sin(thetta1)}, Vec3D{0, 0, 0})); // up plane

    _ready = true;
    Log::log("Camera::init(): camera successfully initialized.");
}

std::vector<Triangle> Camera::sorted() {

    // Sort _tris from back to front
    // This is some replacement for Z-buffer
    // TODO: implemented (lesson 3)

    //используем стандартную функцию сортировки.
    //будем сравнивать средние точки треугольника. (точнее их координату z)
    //(на 3 не делим для нахождения среднего, потому что нам и такой результат сгодится)
    std::sort(_triangles.begin(), _triangles.end(), [](Triangle &t1, Triangle &t2){
        double z1 = t1[0].z() + t1[1].z() + t1[3].z();
        double z2 = t2[0].z() + t2[1].z() + t2[3].z();
        return z1 > z2;
    });
    return _triangles;
}

void Camera::clear() {
    // Cleaning all _tris and recalculation of View matrix
    _triangles.clear(); //АК: метод библиотечного класса std::vector - делает коллекцию длиной 0.
}
