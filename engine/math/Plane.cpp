//
// Created by Иван Ильин on 19.01.2021.
//

#include "Plane.h"

Plane::Plane(const Triangle &tri) : _normal(tri.norm()), _point(tri[0]) {
}

Plane::Plane(const Vec3D &N, const Vec3D &P) : _normal(N.normalized()), _point(P) {
}

//расстояние от точки до плоскости
double Plane::distance(const Vec3D &point) const {
    // TODO: implemented (lesson 4)
    //проекция вектора точки на нормаль плоскоси  
    //минус
    //проеция вектора точки плоскости на нормаль плоскости
     return point.dot(_normal) - _point.dot(_normal);
}

//находим точку пересечения прямой с плоскостью
//передаем две точки прямой.
//возвращаем пару:
//точу пересечения прямой с плоскостью,
//коэффициент k - отношение расстояния от точки start до плоскости  к  длинне отрезка Start-end. (от 0 - до 1 - если точки лежат по обе стороны плоскости)
//зачем нам нужен этот коэффициент - пока непонятно.
std::pair<Vec3D, double> Plane::intersection(const Vec3D &start, const Vec3D &end) const {

    // TODO: implemented (lesson 4)
    double s_dot_n = start.dot(_normal);
    double k = (s_dot_n - _point.dot(_normal)) / (s_dot_n - end.dot(_normal));
    Vec3D res = start + (end - start) * k;
    return std::make_pair(res, k);
}

//функция клиппнга.
//принимает треугольник, 
//возвращает - коллекцию треугольников.
std::vector<Triangle> Plane::clip(const Triangle &tri) const {

    std::vector<Triangle> result;

    std::vector<Vec3D> insidePoints;
    std::vector<Vec3D> outsidePoints;

    double distances[3] = {distance(Vec3D(tri[0])),
                           distance(Vec3D(tri[1])),
                           distance(Vec3D(tri[2]))};

//АК комент:
//видимо нормали плоскостей обрезающих коробку области видимости
//расположены внутрь коробки области видимости.
//поэтому те точки которые расположены со стороны нормали плоскости (расстояние до плосткости > 0)
//мы складываем в коллекцию insidePoints - внутренние точки. Внутри области видимости.
//Точки у которых расстояние до плоскости отрицательные - это точки расположенные снаружи коробки области видимости,
//поэтому мы их кладем в коллекцию outsidePoints.
    for (int i = 0; i < 3; i++) {
        if (distances[i] >= 0) {
            insidePoints.emplace_back(tri[i]);
        } else {
            outsidePoints.emplace_back(tri[i]);
        }
    }

    if (insidePoints.size() == 1) {
        // TODO: implemented (lesson 4)
        auto intersect1 = intersection(insidePoints[0], outsidePoints[0]);
        auto intersect2 = intersection(insidePoints[0], outsidePoints[1]);

        result.emplace_back(insidePoints[0].makePoint4D(),
                            intersect1.first.makePoint4D(),
                            intersect2.first.makePoint4D(),
                            //tri.color());
                            sf::Color(0,0,255));//если отрезался и получился один треугольник - закрашиваем в синий цвет
    }

    if (insidePoints.size() == 2) {
        // TODO: implement (lesson 4)
        auto intersect1 = intersection(insidePoints[0], outsidePoints[0]);
        auto intersect2 = intersection(insidePoints[1], outsidePoints[0]);

        result.emplace_back(insidePoints[0].makePoint4D(),
                            intersect1.first.makePoint4D(),
                            insidePoints[1].makePoint4D(),
                            //tri.color());
                            sf::Color(255,0,0));//если отрезался и получилось 2 треугольника - закрашиваем в красный цвет

        result.emplace_back(intersect1.first.makePoint4D(),
                            intersect2.first.makePoint4D(),
                            insidePoints[1].makePoint4D(),
                            //tri.color());
                            sf::Color(255,0,0));//если отрезался и получилось 2 треугольника - закрашиваем в красный цвет
    }

    if (insidePoints.size() == 3) {
        // TODO: implemented (lesson 4)
        result.emplace_back(tri);
    }

    return result;
}
