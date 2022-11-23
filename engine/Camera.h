//
// Created by Иван Ильин on 14.01.2021.
//

#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <vector>

#include <SFML/OpenGL.hpp>

#include "math/Plane.h"
#include "Mesh.h"

class Camera final : public Object {
private:
    std::vector<Triangle> _triangles{};
    std::vector<Plane> _clipPlanes{};
    bool _ready = false;
    double _aspect = 0;

    Matrix4x4 _SP;
public:
    Camera() : Object(ObjectNameTag("Camera")) {};

    Camera(const Camera &camera) = delete;
    //АК комент: 90 градусов угол обзора - это очень много. 
    //это примерно как полнокадровый фотоаппарат с объективом с фокусным расстоянием 14 - 20 мм
    //Для человека(меня) нормальный угол обзора - 40 градусов
    //т.е. как полнокадровый фотоаппарат с объективом с фокусным расстоянием 50мм.
    //Фотоаппарат с кроп-матрицей дает коэффициент 1,5.
    //тогда у фотоппарата с кроп-матрицей и объективом с фокусным расстоянием 50мм будет угол обзора не 40 градусов,
    //а примерно 28 градусов. (50*1,5=75 т.е. как у полнокадрового с объетивом 75мм)
    //https://vk.com/@camerawars-kakov-realnyi-ugol-obzora-kamery-uglovoe-pole
    void init(int width, int height, double fov = 90.0, double ZNear = 0.1, double ZFar = 100.0);
    //void init(int width, int height, double fov = 40.0, double ZNear = 0.1, double ZFar = 100.0);
    //void init(int width, int height, double fov = 28.0, double ZNear = 0.1, double ZFar = 100.0);

    std::vector<Triangle> project(std::shared_ptr<Mesh> mesh);

    void clear();

    [[nodiscard]] int buffSize() const { return _triangles.size(); }

    std::vector<Triangle> sorted();

};


#endif //INC_3DZAVR_CAMERA_H
