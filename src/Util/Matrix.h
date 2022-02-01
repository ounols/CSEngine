#pragma once

#include "Vector.h"
#include "MatrixDef.h"

namespace CSE {

    template <typename T>
    class Matrix2 {
    public:
        //    T m_mat[4] = {};
        vec2 x;
        vec2 y;
    public:
        Matrix2() {
            MAT2_XX = 1;
            MAT2_XY = 0;
            MAT2_YX = 0;
            MAT2_YY = 1;
        }

        explicit Matrix2(const T* m) {
            MAT2_XX = m[0];
            MAT2_XY = m[1];
            MAT2_YX = m[2];
            MAT2_YY = m[3];
        }

        ~Matrix2() = default;
        const T* Pointer() const {
            return &MAT2_XX;
        }
    };

    template <typename T>
    class Matrix3 {
    public:
        //    T m_mat[9] = {};
        vec3 x;
        vec3 y;
        vec3 z;

        Matrix3() {
            MAT3_XX = 1;
            MAT3_XY = 0;
            MAT3_XZ = 0;
            MAT3_YX = 0;
            MAT3_YY = 1;
            MAT3_YZ = 0;
            MAT3_ZX = 0;
            MAT3_ZY = 0;
            MAT3_ZZ = 1;
        }

        explicit Matrix3(const T* m) {
            MAT3_XX = m[0];
            MAT3_XY = m[1];
            MAT3_XZ = m[2];
            MAT3_YX = m[3];
            MAT3_YY = m[4];
            MAT3_YZ = m[5];
            MAT3_ZX = m[6];
            MAT3_ZY = m[7];
            MAT3_ZZ = m[8];
        }

        ~Matrix3() = default;

        Matrix3 Transposed() const {
            Matrix3 m;
            m.MAT3_XX = MAT3_XX;
            m.MAT3_XY = MAT3_YX;
            m.MAT3_XZ = MAT3_ZX;
            m.MAT3_YX = MAT3_XY;
            m.MAT3_YY = MAT3_YY;
            m.MAT3_YZ = MAT3_ZY;
            m.MAT3_ZX = MAT3_XZ;
            m.MAT3_ZY = MAT3_YZ;
            m.MAT3_ZZ = MAT3_ZZ;
            return m;
        }

        Vector3<T> operator*(const Vector3<T>& b) const {
            Vector3<T> v;
            v.x = MAT3_XX * b.x + MAT3_XY * b.y + MAT3_XZ * b.z;
            v.y = MAT3_YX * b.x + MAT3_YY * b.y + MAT3_YZ * b.z;
            v.z = MAT3_ZX * b.x + MAT3_ZY * b.y + MAT3_ZZ * b.z;
            return v;
        }

        const T* Pointer() const {
            return &MAT3_XX;
        }
    };

    template <typename T>
    class Matrix4 {
    public:
        //    T m_mat[16] = {};
        vec4 x;
        vec4 y;
        vec4 z;
        vec4 w;

        Matrix4() {
            MAT4_XX = 1;
            MAT4_XY = 0;
            MAT4_XZ = 0;
            MAT4_XW = 0;
            MAT4_YX = 0;
            MAT4_YY = 1;
            MAT4_YZ = 0;
            MAT4_YW = 0;
            MAT4_ZX = 0;
            MAT4_ZY = 0;
            MAT4_ZZ = 1;
            MAT4_ZW = 0;
            MAT4_WX = 0;
            MAT4_WY = 0;
            MAT4_WZ = 0;
            MAT4_WW = 1;
        }

        explicit Matrix4(const Matrix3<T>& m) {
            MAT4_XX = m.MAT4_XX;
            MAT4_XY = m.MAT4_XY;
            MAT4_XZ = m.MAT4_XZ;
            MAT4_XW = 0;
            MAT4_YX = m.MAT4_YX;
            MAT4_YY = m.MAT4_YY;
            MAT4_YZ = m.MAT4_YZ;
            MAT4_YW = 0;
            MAT4_ZX = m.MAT4_ZX;
            MAT4_ZY = m.MAT4_ZY;
            MAT4_ZZ = m.MAT4_ZZ;
            MAT4_ZW = 0;
            MAT4_WX = 0;
            MAT4_WY = 0;
            MAT4_WZ = 0;
            MAT4_WW = 1;
        }

        explicit Matrix4(const T* m) {
            MAT4_XX = m[0];
            MAT4_XY = m[1];
            MAT4_XZ = m[2];
            MAT4_XW = m[3];
            MAT4_YX = m[4];
            MAT4_YY = m[5];
            MAT4_YZ = m[6];
            MAT4_YW = m[7];
            MAT4_ZX = m[8];
            MAT4_ZY = m[9];
            MAT4_ZZ = m[10];
            MAT4_ZW = m[11];
            MAT4_WX = m[12];
            MAT4_WY = m[13];
            MAT4_WZ = m[14];
            MAT4_WW = m[15];
        }

        ~Matrix4() = default;

        Matrix4 operator*(const Matrix4& b) const {
            Matrix4 m;
            m.MAT4_XX = MAT4_XX * b.MAT4_XX + MAT4_XY * b.MAT4_YX + MAT4_XZ * b.MAT4_ZX + MAT4_XW * b.MAT4_WX;
            m.MAT4_XY = MAT4_XX * b.MAT4_XY + MAT4_XY * b.MAT4_YY + MAT4_XZ * b.MAT4_ZY + MAT4_XW * b.MAT4_WY;
            m.MAT4_XZ = MAT4_XX * b.MAT4_XZ + MAT4_XY * b.MAT4_YZ + MAT4_XZ * b.MAT4_ZZ + MAT4_XW * b.MAT4_WZ;
            m.MAT4_XW = MAT4_XX * b.MAT4_XW + MAT4_XY * b.MAT4_YW + MAT4_XZ * b.MAT4_ZW + MAT4_XW * b.MAT4_WW;
            m.MAT4_YX = MAT4_YX * b.MAT4_XX + MAT4_YY * b.MAT4_YX + MAT4_YZ * b.MAT4_ZX + MAT4_YW * b.MAT4_WX;
            m.MAT4_YY = MAT4_YX * b.MAT4_XY + MAT4_YY * b.MAT4_YY + MAT4_YZ * b.MAT4_ZY + MAT4_YW * b.MAT4_WY;
            m.MAT4_YZ = MAT4_YX * b.MAT4_XZ + MAT4_YY * b.MAT4_YZ + MAT4_YZ * b.MAT4_ZZ + MAT4_YW * b.MAT4_WZ;
            m.MAT4_YW = MAT4_YX * b.MAT4_XW + MAT4_YY * b.MAT4_YW + MAT4_YZ * b.MAT4_ZW + MAT4_YW * b.MAT4_WW;
            m.MAT4_ZX = MAT4_ZX * b.MAT4_XX + MAT4_ZY * b.MAT4_YX + MAT4_ZZ * b.MAT4_ZX + MAT4_ZW * b.MAT4_WX;
            m.MAT4_ZY = MAT4_ZX * b.MAT4_XY + MAT4_ZY * b.MAT4_YY + MAT4_ZZ * b.MAT4_ZY + MAT4_ZW * b.MAT4_WY;
            m.MAT4_ZZ = MAT4_ZX * b.MAT4_XZ + MAT4_ZY * b.MAT4_YZ + MAT4_ZZ * b.MAT4_ZZ + MAT4_ZW * b.MAT4_WZ;
            m.MAT4_ZW = MAT4_ZX * b.MAT4_XW + MAT4_ZY * b.MAT4_YW + MAT4_ZZ * b.MAT4_ZW + MAT4_ZW * b.MAT4_WW;
            m.MAT4_WX = MAT4_WX * b.MAT4_XX + MAT4_WY * b.MAT4_YX + MAT4_WZ * b.MAT4_ZX + MAT4_WW * b.MAT4_WX;
            m.MAT4_WY = MAT4_WX * b.MAT4_XY + MAT4_WY * b.MAT4_YY + MAT4_WZ * b.MAT4_ZY + MAT4_WW * b.MAT4_WY;
            m.MAT4_WZ = MAT4_WX * b.MAT4_XZ + MAT4_WY * b.MAT4_YZ + MAT4_WZ * b.MAT4_ZZ + MAT4_WW * b.MAT4_WZ;
            m.MAT4_WW = MAT4_WX * b.MAT4_XW + MAT4_WY * b.MAT4_YW + MAT4_WZ * b.MAT4_ZW + MAT4_WW * b.MAT4_WW;
            return m;
        }

        Vector4<T> operator*(const Vector4<T>& b) const {
            Vector4<T> v;
            v.x = MAT4_XX * b.x + MAT4_XY * b.y + MAT4_XZ * b.z + MAT4_XW * b.w;
            v.y = MAT4_YX * b.x + MAT4_YY * b.y + MAT4_YZ * b.z + MAT4_YW * b.w;
            v.z = MAT4_ZX * b.x + MAT4_ZY * b.y + MAT4_ZZ * b.z + MAT4_ZW * b.w;
            v.w = MAT4_WX * b.x + MAT4_WY * b.y + MAT4_WZ * b.z + MAT4_WW * b.w;
            return v;
        }

        Matrix4& operator*=(const Matrix4& b) {
            Matrix4 m = *this * b;
            return (*this = m);
        }

        Matrix4 Transposed() const {
            Matrix4 m;
            m.MAT4_XX = MAT4_XX;
            m.MAT4_XY = MAT4_YX;
            m.MAT4_XZ = MAT4_ZX;
            m.MAT4_XW = MAT4_WX;
            m.MAT4_YX = MAT4_XY;
            m.MAT4_YY = MAT4_YY;
            m.MAT4_YZ = MAT4_ZY;
            m.MAT4_YW = MAT4_WY;
            m.MAT4_ZX = MAT4_XZ;
            m.MAT4_ZY = MAT4_YZ;
            m.MAT4_ZZ = MAT4_ZZ;
            m.MAT4_ZW = MAT4_WZ;
            m.MAT4_WX = MAT4_XW;
            m.MAT4_WY = MAT4_YW;
            m.MAT4_WZ = MAT4_ZW;
            m.MAT4_WW = MAT4_WW;
            return m;
        }

        Matrix3<T> ToMat3() const {
            Matrix3<T> m;
            m.MAT3_XX = MAT4_XX;
            m.MAT3_YX = MAT4_YX;
            m.MAT3_ZX = MAT4_ZX;
            m.MAT3_XY = MAT4_XY;
            m.MAT3_YY = MAT4_YY;
            m.MAT3_ZY = MAT4_ZY;
            m.MAT3_XZ = MAT4_XZ;
            m.MAT3_YZ = MAT4_YZ;
            m.MAT3_ZZ = MAT4_ZZ;
            return m;
        }

        const T* Pointer() const {
            return &MAT4_XX;
        }

        static Matrix4<T> Identity() {
            return Matrix4();
        }

        static Matrix4<T> Translate(T x, T y, T z) {
            Matrix4 m;
            m.MAT4_XX = 1;
            m.MAT4_XY = 0;
            m.MAT4_XZ = 0;
            m.MAT4_XW = 0;
            m.MAT4_YX = 0;
            m.MAT4_YY = 1;
            m.MAT4_YZ = 0;
            m.MAT4_YW = 0;
            m.MAT4_ZX = 0;
            m.MAT4_ZY = 0;
            m.MAT4_ZZ = 1;
            m.MAT4_ZW = 0;
            m.MAT4_WX = x;
            m.MAT4_WY = y;
            m.MAT4_WZ = z;
            m.MAT4_WW = 1;
            return m;
        }

        static Matrix4<T> Scale(T s) {
            Matrix4 m;
            m.MAT4_XX = s;
            m.MAT4_XY = 0;
            m.MAT4_XZ = 0;
            m.MAT4_XW = 0;
            m.MAT4_YX = 0;
            m.MAT4_YY = s;
            m.MAT4_YZ = 0;
            m.MAT4_YW = 0;
            m.MAT4_ZX = 0;
            m.MAT4_ZY = 0;
            m.MAT4_ZZ = s;
            m.MAT4_ZW = 0;
            m.MAT4_WX = 0;
            m.MAT4_WY = 0;
            m.MAT4_WZ = 0;
            m.MAT4_WW = 1;
            return m;
        }

        static Matrix4<T> Scale(T x, T y, T z) {
            Matrix4 m;
            m.MAT4_XX = x;
            m.MAT4_XY = 0;
            m.MAT4_XZ = 0;
            m.MAT4_XW = 0;
            m.MAT4_YX = 0;
            m.MAT4_YY = y;
            m.MAT4_YZ = 0;
            m.MAT4_YW = 0;
            m.MAT4_ZX = 0;
            m.MAT4_ZY = 0;
            m.MAT4_ZZ = z;
            m.MAT4_ZW = 0;
            m.MAT4_WX = 0;
            m.MAT4_WY = 0;
            m.MAT4_WZ = 0;
            m.MAT4_WW = 1;
            return m;
        }

        static Matrix4<T> RotateZ(T degrees) {
            T radians = degrees * 3.14159f / 180.0f;
            T s = std::sin(radians);
            T c = std::cos(radians);

            Matrix4 m;
            m.MAT4_XX = c;
            m.MAT4_XY = -s;
            m.MAT4_XZ = 0;
            m.MAT4_XW = 0;
            m.MAT4_YX = s;
            m.MAT4_YY = c;
            m.MAT4_YZ = 0;
            m.MAT4_YW = 0;
            m.MAT4_ZX = 0;
            m.MAT4_ZY = 0;
            m.MAT4_ZZ = 1;
            m.MAT4_ZW = 0;
            m.MAT4_WX = 0;
            m.MAT4_WY = 0;
            m.MAT4_WZ = 0;
            m.MAT4_WW = 1;
            return m;
        }


        static Matrix4<T> RotateY(T degrees) {
            T radians = degrees * 3.14159f / 180.0f;
            T s = std::sin(radians);
            T c = std::cos(radians);

            Matrix4 m;
            m.MAT4_XX = c;
            m.MAT4_XY = 0;
            m.MAT4_XZ = s;
            m.MAT4_XW = 0;
            m.MAT4_YX = 0;
            m.MAT4_YY = 1;
            m.MAT4_YZ = 0;
            m.MAT4_YW = 0;
            m.MAT4_ZX = -s;
            m.MAT4_ZY = 0;
            m.MAT4_ZZ = c;
            m.MAT4_ZW = 0;
            m.MAT4_WX = 0;
            m.MAT4_WY = 0;
            m.MAT4_WZ = 0;
            m.MAT4_WW = 1;
            return m;
        }

        static Matrix4<T> RotateX(T degrees) {
            T radians = degrees * 3.14159f / 180.0f;
            T s = std::sin(radians);
            T c = std::cos(radians);

            Matrix4 m;
            m.MAT4_XX = 1;
            m.MAT4_XY = 0;
            m.MAT4_XZ = 0;
            m.MAT4_XW = 0;
            m.MAT4_YX = 0;
            m.MAT4_YY = c;
            m.MAT4_YZ = -s;
            m.MAT4_YW = 0;
            m.MAT4_ZX = 0;
            m.MAT4_ZY = s;
            m.MAT4_ZZ = c;
            m.MAT4_ZW = 0;
            m.MAT4_WX = 0;
            m.MAT4_WY = 0;
            m.MAT4_WZ = 0;
            m.MAT4_WW = 1;
            return m;
        }

        static Matrix4<T> Invert(Matrix4<T> m) {
            Matrix4<T> inv;
            T det;

            inv.MAT4_XX = m.MAT4_YY * m.MAT4_ZZ * m.MAT4_WW -
                          m.MAT4_YY * m.MAT4_ZW * m.MAT4_WZ -
                          m.MAT4_ZY * m.MAT4_YZ * m.MAT4_WW +
                          m.MAT4_ZY * m.MAT4_YW * m.MAT4_WZ +
                          m.MAT4_WY * m.MAT4_YZ * m.MAT4_ZW -
                          m.MAT4_WY * m.MAT4_YW * m.MAT4_ZZ;

            inv.MAT4_YX = -m.MAT4_YX * m.MAT4_ZZ * m.MAT4_WW +
                          m.MAT4_YX * m.MAT4_ZW * m.MAT4_WZ +
                          m.MAT4_ZX * m.MAT4_YZ * m.MAT4_WW -
                          m.MAT4_ZX * m.MAT4_YW * m.MAT4_WZ -
                          m.MAT4_WX * m.MAT4_YZ * m.MAT4_ZW +
                          m.MAT4_WX * m.MAT4_YW * m.MAT4_ZZ;

            inv.MAT4_ZX = m.MAT4_YX * m.MAT4_ZY * m.MAT4_WW -
                          m.MAT4_YX * m.MAT4_ZW * m.MAT4_WY -
                          m.MAT4_ZX * m.MAT4_YY * m.MAT4_WW +
                          m.MAT4_ZX * m.MAT4_YW * m.MAT4_WY +
                          m.MAT4_WX * m.MAT4_YY * m.MAT4_ZW -
                          m.MAT4_WX * m.MAT4_YW * m.MAT4_ZY;

            inv.MAT4_WX = -m.MAT4_YX * m.MAT4_ZY * m.MAT4_WZ +
                          m.MAT4_YX * m.MAT4_ZZ * m.MAT4_WY +
                          m.MAT4_ZX * m.MAT4_YY * m.MAT4_WZ -
                          m.MAT4_ZX * m.MAT4_YZ * m.MAT4_WY -
                          m.MAT4_WX * m.MAT4_YY * m.MAT4_ZZ +
                          m.MAT4_WX * m.MAT4_YZ * m.MAT4_ZY;

            inv.MAT4_XY = -m.MAT4_XY * m.MAT4_ZZ * m.MAT4_WW +
                          m.MAT4_XY * m.MAT4_ZW * m.MAT4_WZ +
                          m.MAT4_ZY * m.MAT4_XZ * m.MAT4_WW -
                          m.MAT4_ZY * m.MAT4_XW * m.MAT4_WZ -
                          m.MAT4_WY * m.MAT4_XZ * m.MAT4_ZW +
                          m.MAT4_WY * m.MAT4_XW * m.MAT4_ZZ;

            inv.MAT4_YY = m.MAT4_XX * m.MAT4_ZZ * m.MAT4_WW -
                          m.MAT4_XX * m.MAT4_ZW * m.MAT4_WZ -
                          m.MAT4_ZX * m.MAT4_XZ * m.MAT4_WW +
                          m.MAT4_ZX * m.MAT4_XW * m.MAT4_WZ +
                          m.MAT4_WX * m.MAT4_XZ * m.MAT4_ZW -
                          m.MAT4_WX * m.MAT4_XW * m.MAT4_ZZ;

            inv.MAT4_ZY = -m.MAT4_XX * m.MAT4_ZY * m.MAT4_WW +
                          m.MAT4_XX * m.MAT4_ZW * m.MAT4_WY +
                          m.MAT4_ZX * m.MAT4_XY * m.MAT4_WW -
                          m.MAT4_ZX * m.MAT4_XW * m.MAT4_WY -
                          m.MAT4_WX * m.MAT4_XY * m.MAT4_ZW +
                          m.MAT4_WX * m.MAT4_XW * m.MAT4_ZY;

            inv.MAT4_WY = m.MAT4_XX * m.MAT4_ZY * m.MAT4_WZ -
                          m.MAT4_XX * m.MAT4_ZZ * m.MAT4_WY -
                          m.MAT4_ZX * m.MAT4_XY * m.MAT4_WZ +
                          m.MAT4_ZX * m.MAT4_XZ * m.MAT4_WY +
                          m.MAT4_WX * m.MAT4_XY * m.MAT4_ZZ -
                          m.MAT4_WX * m.MAT4_XZ * m.MAT4_ZY;

            inv.MAT4_XZ = m.MAT4_XY * m.MAT4_YZ * m.MAT4_WW -
                          m.MAT4_XY * m.MAT4_YW * m.MAT4_WZ -
                          m.MAT4_YY * m.MAT4_XZ * m.MAT4_WW +
                          m.MAT4_YY * m.MAT4_XW * m.MAT4_WZ +
                          m.MAT4_WY * m.MAT4_XZ * m.MAT4_YW -
                          m.MAT4_WY * m.MAT4_XW * m.MAT4_YZ;

            inv.MAT4_YZ = -m.MAT4_XX * m.MAT4_YZ * m.MAT4_WW +
                          m.MAT4_XX * m.MAT4_YW * m.MAT4_WZ +
                          m.MAT4_YX * m.MAT4_XZ * m.MAT4_WW -
                          m.MAT4_YX * m.MAT4_XW * m.MAT4_WZ -
                          m.MAT4_WX * m.MAT4_XZ * m.MAT4_YW +
                          m.MAT4_WX * m.MAT4_XW * m.MAT4_YZ;

            inv.MAT4_ZZ = m.MAT4_XX * m.MAT4_YY * m.MAT4_WW -
                          m.MAT4_XX * m.MAT4_YW * m.MAT4_WY -
                          m.MAT4_YX * m.MAT4_XY * m.MAT4_WW +
                          m.MAT4_YX * m.MAT4_XW * m.MAT4_WY +
                          m.MAT4_WX * m.MAT4_XY * m.MAT4_YW -
                          m.MAT4_WX * m.MAT4_XW * m.MAT4_YY;

            inv.MAT4_WZ = -m.MAT4_XX * m.MAT4_YY * m.MAT4_WZ +
                          m.MAT4_XX * m.MAT4_YZ * m.MAT4_WY +
                          m.MAT4_YX * m.MAT4_XY * m.MAT4_WZ -
                          m.MAT4_YX * m.MAT4_XZ * m.MAT4_WY -
                          m.MAT4_WX * m.MAT4_XY * m.MAT4_YZ +
                          m.MAT4_WX * m.MAT4_XZ * m.MAT4_YY;

            inv.MAT4_XW = -m.MAT4_XY * m.MAT4_YZ * m.MAT4_ZW +
                          m.MAT4_XY * m.MAT4_YW * m.MAT4_ZZ +
                          m.MAT4_YY * m.MAT4_XZ * m.MAT4_ZW -
                          m.MAT4_YY * m.MAT4_XW * m.MAT4_ZZ -
                          m.MAT4_ZY * m.MAT4_XZ * m.MAT4_YW +
                          m.MAT4_ZY * m.MAT4_XW * m.MAT4_YZ;

            inv.MAT4_YW = m.MAT4_XX * m.MAT4_YZ * m.MAT4_ZW -
                          m.MAT4_XX * m.MAT4_YW * m.MAT4_ZZ -
                          m.MAT4_YX * m.MAT4_XZ * m.MAT4_ZW +
                          m.MAT4_YX * m.MAT4_XW * m.MAT4_ZZ +
                          m.MAT4_ZX * m.MAT4_XZ * m.MAT4_YW -
                          m.MAT4_ZX * m.MAT4_XW * m.MAT4_YZ;

            inv.MAT4_ZW = -m.MAT4_XX * m.MAT4_YY * m.MAT4_ZW +
                          m.MAT4_XX * m.MAT4_YW * m.MAT4_ZY +
                          m.MAT4_YX * m.MAT4_XY * m.MAT4_ZW -
                          m.MAT4_YX * m.MAT4_XW * m.MAT4_ZY -
                          m.MAT4_ZX * m.MAT4_XY * m.MAT4_YW +
                          m.MAT4_ZX * m.MAT4_XW * m.MAT4_YY;

            inv.MAT4_WW = m.MAT4_XX * m.MAT4_YY * m.MAT4_ZZ -
                          m.MAT4_XX * m.MAT4_YZ * m.MAT4_ZY -
                          m.MAT4_YX * m.MAT4_XY * m.MAT4_ZZ +
                          m.MAT4_YX * m.MAT4_XZ * m.MAT4_ZY +
                          m.MAT4_ZX * m.MAT4_XY * m.MAT4_YZ -
                          m.MAT4_ZX * m.MAT4_XZ * m.MAT4_YY;

            det = m.MAT4_XX * inv.MAT4_XX + m.MAT4_XY * inv.MAT4_YX + m.MAT4_XZ * inv.MAT4_ZX + m.MAT4_XW * inv.MAT4_WX;

            if (det == 0)
                return Identity();

            det = 1.0 / det;

            Matrix4<T> invOut;

            invOut.MAT4_XX = inv.MAT4_XX * det;
            invOut.MAT4_XY = inv.MAT4_XY * det;
            invOut.MAT4_XZ = inv.MAT4_XZ * det;
            invOut.MAT4_XW = inv.MAT4_XW * det;

            invOut.MAT4_YX = inv.MAT4_YX * det;
            invOut.MAT4_YY = inv.MAT4_YY * det;
            invOut.MAT4_YZ = inv.MAT4_YZ * det;
            invOut.MAT4_YW = inv.MAT4_YW * det;

            invOut.MAT4_ZX = inv.MAT4_ZX * det;
            invOut.MAT4_ZY = inv.MAT4_ZY * det;
            invOut.MAT4_ZZ = inv.MAT4_ZZ * det;
            invOut.MAT4_ZW = inv.MAT4_ZW * det;

            invOut.MAT4_WX = inv.MAT4_WX * det;
            invOut.MAT4_WY = inv.MAT4_WY * det;
            invOut.MAT4_WZ = inv.MAT4_WZ * det;
            invOut.MAT4_WW = inv.MAT4_WW * det;

            return invOut;
        }

        static Matrix4<T> Frustum(T tLeft, T tRight, T tBottom, T tTop, T tNear, T tFar) {
            T a = 2 * tNear / (tRight - tLeft);
            T b = 2 * tNear / (tTop - tBottom);
            T c = (tRight + tLeft) / (tRight - tLeft);
            T d = (tTop + tBottom) / (tTop - tBottom);
            T e = -(tFar + tNear) / (tFar - tNear);
            T f = -2 * tFar * tNear / (tFar - tNear);
            Matrix4 m;
            m.MAT4_XX = a;
            m.MAT4_XY = 0;
            m.MAT4_XZ = 0;
            m.MAT4_XW = 0;
            m.MAT4_YX = 0;
            m.MAT4_YY = b;
            m.MAT4_YZ = 0;
            m.MAT4_YW = 0;
            m.MAT4_ZX = c;
            m.MAT4_ZY = d;
            m.MAT4_ZZ = e;
            m.MAT4_ZW = -1;
            m.MAT4_WX = 0;
            m.MAT4_WY = 0;
            m.MAT4_WZ = f;
            m.MAT4_WW = 1;
            return m;
        }

        static Matrix4<T> Perspective(T fovy, T aspect, T tNear, T tFar) {
            Matrix4 m;
            T sine, cosine, cotangent, deltaZ;
            T radians = (T) (fovy / 2.0f * Pi / 180.0f);

            deltaZ = (T) (tFar - tNear);
            sine = (T) sin(radians);
            cosine = (T) cos(radians);

            if ((deltaZ == 0.0f) || (sine == 0.0f) || (aspect == 0.0f)) {
                return m;
            }

            cotangent = (T) (cosine / sine);

            m.MAT4_XX = cotangent / aspect;
            m.MAT4_YY = cotangent;
            m.MAT4_ZZ = -(tFar + tNear) / deltaZ;
            m.MAT4_ZW = -1.0f;
            m.MAT4_WZ = -2.0f * tNear * tFar / deltaZ;
            m.MAT4_WW = 0.0f;

            return m;
        }

        static Matrix4<T> Ortho(T xMin, T xMax, T yMin, T yMax, T zMin, T zMax) {
            Matrix4 m;

            T xDiff = xMax - xMin;
            T yDiff = yMax - yMin;
            T zDiff = zMax - zMin;

            m.MAT4_XX = 2.0f / xDiff;
            m.MAT4_YY = 2.0f / yDiff;
            m.MAT4_ZZ = -2.0f / zDiff;

            m.MAT4_WX = -(xMax + xMin) / xDiff;
            m.MAT4_WY = -(yMax + yMin) / yDiff;
            m.MAT4_WZ = -(zMax + zMin) / zDiff;
            m.MAT4_WW = 1.0f;

            return m;
        }

        static Matrix4<T> LookAt(const Vector3<T>& eye,
                                 const Vector3<T>& target,
                                 const Vector3<T>& up) {
            Vector3<T> z = (eye - target).Normalized();
            Vector3<T> x = up.Cross(z).Normalized();
            Vector3<T> y = z.Cross(x).Normalized();

            Matrix4<T> m;

            m.MAT4_XX = x.x;
            m.MAT4_XY = x.y;
            m.MAT4_XZ = x.z;
            m.MAT4_XW = 0;
            m.MAT4_YX = y.x;
            m.MAT4_YY = y.y;
            m.MAT4_YZ = y.z;
            m.MAT4_YW = 0;
            m.MAT4_ZX = z.x;
            m.MAT4_ZY = z.y;
            m.MAT4_ZZ = z.z;
            m.MAT4_ZW = 0;
            m.MAT4_WX = 0;
            m.MAT4_WY = 0;
            m.MAT4_WZ = 0;
            m.MAT4_WW = 1;

            Vector4<T> eyePrime = m * Vector4<T>(-eye, 1);
            m = m.Transposed();
            m.w = eyePrime;
            return m;
        }
    };

    typedef Matrix2<float> mat2;
    typedef Matrix3<float> mat3;
    typedef Matrix4<float> mat4;
}