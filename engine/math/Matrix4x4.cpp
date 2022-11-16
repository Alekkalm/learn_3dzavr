//
// Created by Иван Ильин on 12.01.2021.
//

#include <cmath>
#include <cassert>

#include "Matrix4x4.h"
#include "../Consts.h"
//умножение матрицы на матрицу
Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &matrix4X4) const {
    Matrix4x4 result = Matrix4x4::Zero();

    for(int i = 0; i < 4; ++i){
        for(int j = 0; j < 4; ++j){
            for(int k = 0; k < 4; ++k){
                result._arr[i][j] += _arr[i][k] * matrix4X4._arr[k][j];
            }
        }
    }
    return result;// TODO: implemented  (lesson 1)
}
//умножение матицы на 4D-вектор
Vec4D Matrix4x4::operator*(const Vec4D &point4D) const {
    return Vec4D(
        _arr[0][0]*point4D.x() + _arr[0][1]*point4D.y() + _arr[0][2]*point4D.z() + _arr[0][3]*point4D.w(),
        _arr[1][0]*point4D.x() + _arr[1][1]*point4D.y() + _arr[1][2]*point4D.z() + _arr[1][3]*point4D.w(),
        _arr[2][0]*point4D.x() + _arr[2][1]*point4D.y() + _arr[2][2]*point4D.z() + _arr[2][3]*point4D.w(),
        _arr[3][0]*point4D.x() + _arr[3][1]*point4D.y() + _arr[3][2]*point4D.z() + _arr[3][3]*point4D.w()
    );// TODO: implemented (lesson 1)
}
//умножение матрицы на 3D-вектор
Vec3D Matrix4x4::operator*(const Vec3D &vec) const {
    return Vec3D(
        _arr[0][0]*vec.x() + _arr[0][1]*vec.y() + _arr[0][2]*vec.z(),
        _arr[1][0]*vec.x() + _arr[1][1]*vec.y() + _arr[1][2]*vec.z(),
        _arr[2][0]*vec.x() + _arr[2][1]*vec.y() + _arr[2][2]*vec.z()
    );// TODO: implemented (lesson 1)
}
//единичная матрица
Matrix4x4 Matrix4x4::Identity() {
    Matrix4x4 result = Matrix4x4::Zero();
    result._arr[0][0] = 1;
    result._arr[1][1] = 1;
    result._arr[2][2] = 1;
    result._arr[3][3] = 1;
    return result;// TODO: implemented (lesson 1)
}
//матрица у которой все элементы равны одному и тому же заданному числу.
Matrix4x4 Matrix4x4::Constant(double value) {
    Matrix4x4 result;
    for(int i = 0; i < 4; ++i){
        for(int j = 0; j < 4; ++j){
            result._arr[i][j] = value;
        }
    }
    return result;// TODO: implemented (lesson 1)
}
//нулевая матрица
Matrix4x4 Matrix4x4::Zero() {
    return Matrix4x4::Constant(0);
}
//задаем матрицу масштабирования тремя числами, передавая их виде вектора.
Matrix4x4 Matrix4x4::Scale(const Vec3D &factor) {
    Matrix4x4 s{};
    s._arr[0][0] = factor.x();
    s._arr[1][1] = factor.y();
    s._arr[2][2] = factor.z();
    s._arr[3][3] = 1;
    return s;// TODO: implemented (lesson 1)
}
//матрица смещения(перемещения) (по диагонали - единицы, последний столбец - цифры смещения)
Matrix4x4 Matrix4x4::Translation(const Vec3D &v) {
    Matrix4x4 t{};
    t._arr[0][0] = 1;
    t._arr[1][1] = 1;
    t._arr[2][2] = 1;
    t._arr[3][3] = 1;

    t._arr[0][3] = v.x();
    t._arr[1][3] = v.y();
    t._arr[2][3] = v.z();
    return t;// TODO: implemented (lesson 1)
}

Matrix4x4 Matrix4x4::RotationX(double rx) {
    Matrix4x4 Rx{};
    double c = cos(rx), s = sin(rx);
    Rx._arr[0][0] = 1.0;

    Rx._arr[1][1] = c;
    Rx._arr[1][2] = -s;
    Rx._arr[2][1] = s;
    Rx._arr[2][2] = c;

    Rx._arr[3][3] = 1.0;
    return Rx;// TODO: implemented (lesson 1)
}

Matrix4x4 Matrix4x4::RotationY(double ry) {
    Matrix4x4 Ry{};
   double c = cos(ry), s = sin(ry);
    Ry._arr[1][1] = 1.0;

    Ry._arr[0][0] = c;
    Ry._arr[0][2] = s;
    Ry._arr[2][0] = -s;
    Ry._arr[2][2] = c;

    Ry._arr[3][3] = 1.0;
    return Ry;// TODO: implemented (lesson 1)
}

Matrix4x4 Matrix4x4::RotationZ(double rz) {
    Matrix4x4 Rz{};
double c = cos(rz), s = sin(rz);
    Rz._arr[2][2] = 1.0;

    Rz._arr[0][0] = c;
    Rz._arr[0][1] = -s;
    Rz._arr[1][0] = s;
    Rz._arr[1][1] = c;

    Rz._arr[3][3] = 1.0;
    return Rz;// TODO: implemented (lesson 1)
}

Matrix4x4 Matrix4x4::Rotation(const Vec3D &r) {
    return RotationX(r.x()) * RotationY(r.y()) * RotationZ(r.z());
}

Matrix4x4 Matrix4x4::Rotation(const Vec3D &v, double rv) {
    Matrix4x4 Rv{};
    Vec3D nv(v.normalized());

    double c = cos(rv), s = sin(rv);

    Rv._arr[0][0] = c + (1.0 - c) * nv.x() * nv.x();
    Rv._arr[0][1] = (1.0 - c) * nv.x() * nv.y() - s * nv.z();
    Rv._arr[0][2] = (1.0 - c) * nv.x() * nv.z() + s * nv.y();

    Rv._arr[1][0] = (1.0 - c) * nv.x() * nv.y() + s * nv.z();
    Rv._arr[1][1] = c + (1.0 - c) * nv.y() * nv.y();
    Rv._arr[1][2] = (1.0 - c) * nv.y() * nv.z() - s * nv.x();

    Rv._arr[2][0] = (1.0 - c) * nv.z() * nv.x() - s * nv.y();
    Rv._arr[2][1] = (1.0 - c) * nv.z() * nv.y() + s * nv.x();
    Rv._arr[2][2] = c + (1.0 - c) * nv.z() * nv.z();

    Rv._arr[3][3] = 1.0;

    return Rv;
}
//матрица проецирования на плоскость (на экран)
//fov - угол обзора полный в градусах
//aspect - соотношение ширины к высоте экрана
//ZNear и ZFar - координаты Z которые после применения матрицы станут 0 и 1. 
//(то что меньше ZNear - станет отрицательным числом, то что больше ZFar - станет числом больше 1.)
//(т.е. это приведение диапазона ZNear - ZFar к диапазону 0 - 1.)
Matrix4x4 Matrix4x4::Projection(double fov, double aspect, double ZNear, double ZFar) {
    Matrix4x4 p{};

    p._arr[0][0] = 1.0/(tan(M_PI * fov * 0.5 /180) * aspect);// TODO: implemented (lesson 2)
    p._arr[1][1] = 1.0/tan(M_PI * fov * 0.5 /180);
    p._arr[2][2] = ZFar / (ZFar - ZNear);
    p._arr[2][3] = -ZFar * ZNear / (ZFar - ZNear);
    p._arr[3][2] = 1;
    return p;
}

//матрица экранного пространства. 
//Выпрямляем оси координат чтобы смотрели в те же стороны как у модели.
//т.к. у экрана 0,0 в левом верхнем углу, а мы делаем чтобы было в центре
//и у экрана - x направлен вправо, а y - вниз,
//а мы делаем как у модели - x напровлен влево, а y - вверх.
//(можно считать что мы переходим от системы координат модели в систему координат экрана)
//(но удобнее думать что мы разварачиваем оси системы координат экрана в соответствии с осями модели (модели нашего мира))
//так же мы масштабируем в пиксели:
//высота нормализованная = 1 будет половина высоты экрана в пикселях.
//ширина нормализованная = 1 будет равна половине ширины экрана в пикселях.
//т.е. мы получаем координаты в пикселях - для отображения на экране.
//z оставляем как есть - направлен от нас.
Matrix4x4 Matrix4x4::ScreenSpace(int width, int height) {
    Matrix4x4 s{};

    s._arr[0][0] = -0.5*width;// TODO: implemented (lesson 2)
    s._arr[1][1] = -0.5*height;

    s._arr[0][3] = 0.5*width;
    s._arr[1][3] = 0.5*height;

    s._arr[2][2] = 1;
    s._arr[3][3] = 1;

    return s;
}

Matrix4x4 Matrix4x4::View(const Vec3D &left, const Vec3D &up, const Vec3D &lookAt, const Vec3D &eye) {
    Matrix4x4 V = Zero();

    // TODO: implement (lesson 4)

    return V;
}

Vec3D Matrix4x4::x() const {
    return Vec3D(_arr[0][0], _arr[1][0], _arr[2][0]);
}

Vec3D Matrix4x4::y() const {
    return Vec3D(_arr[0][1], _arr[1][1], _arr[2][1]);
}

Vec3D Matrix4x4::z() const {
    return Vec3D(_arr[0][2], _arr[1][2], _arr[2][2]);
}

Vec3D Matrix4x4::w() const {
    return Vec3D(_arr[0][3], _arr[1][3], _arr[2][3]);
}

bool Matrix4x4::isNear(double a, double b) {
    return std::abs(a - b) < Consts::EPS;
}

void Matrix4x4::test() {
    // testing matrix-vector multiplication:
    Vec4D v(4, 2, 3, 1);

    Vec4D one = Matrix4x4::Identity()*v;
    assert(isNear(one.x(), 4) && isNear(one.y(), 2) && isNear(one.z(), 3) && isNear(one.w(), 1));

    Vec4D scale = Matrix4x4::Scale(Vec3D(1, 2, 3))*v;
    assert(isNear(scale.x(), 4) && isNear(scale.y(), 4) && isNear(scale.z(), 9) && isNear(scale.w(), 1));

    Vec4D zero = Matrix4x4::Zero()*v;
    assert(isNear(zero.x(), 0) && isNear(zero.y(), 0) && isNear(zero.z(), 0) && isNear(zero.w(), 0));

    Vec4D translated = Matrix4x4::Translation(Vec3D(5, 4, 3))*v;
    assert(isNear(translated.x(), 9) && isNear(translated.y(), 6) && isNear(translated.z(), 6) && isNear(translated.w(), 1));

    // testing matrix-matrix multiplication:
    Matrix4x4 c1 = Matrix4x4::Constant(5)*Matrix4x4::Identity();
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            assert(isNear(c1._arr[i][j], 5));

    Matrix4x4 c2 = Matrix4x4::Zero()*Matrix4x4::Constant(1);
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            assert(isNear(c2._arr[i][j], 0));

    Matrix4x4 c3 = Matrix4x4::Scale(Vec3D(3, 3, 3))*Matrix4x4::Identity();
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            if(i==j)
                if(i<3)
                    assert(isNear(c3._arr[i][j], 3));
                else
                    assert(isNear(c3._arr[i][j], 1));
            else
                assert(isNear(c3._arr[i][j], 0));

    // testing rotation matrices:
    Vec4D i(1, 0,0);
    Vec4D j(0, 1,0);
    Vec4D k(0, 0,1);

    // rotation X:
    Vec4D i_rx = Matrix4x4::RotationX(M_PI/2)*i;
    Vec4D j_rx = Matrix4x4::RotationX(M_PI/2)*j;
    Vec4D k_rx = Matrix4x4::RotationX(M_PI/2)*k;

    assert(i_rx == i);
    assert(j_rx == k);
    assert(k_rx == -j);

    // rotation Y:
    Vec4D i_ry = Matrix4x4::RotationY(M_PI/2)*i;
    Vec4D j_ry = Matrix4x4::RotationY(M_PI/2)*j;
    Vec4D k_ry = Matrix4x4::RotationY(M_PI/2)*k;

    assert(i_ry == -k);
    assert(j_ry == j);
    assert(k_ry == i);

    // rotation Z:
    Vec4D i_rz = Matrix4x4::RotationZ(M_PI/2)*i;
    Vec4D j_rz = Matrix4x4::RotationZ(M_PI/2)*j;
    Vec4D k_rz = Matrix4x4::RotationZ(M_PI/2)*k;

    assert(i_rz == j);
    assert(j_rz == -i);
    assert(k_rz == k);
}
