#pragma once

#include "Matrix.h"
#include "MoreMath.h"

namespace CSE {
    template <typename T>
    struct QuaternionT {
        T x;
        T y;
        T z;
        T w;

        QuaternionT();

        QuaternionT(T x, T y, T z, T w);

        void Set(T x, T y, T z, T w);

        QuaternionT<T> Clone() const;

        QuaternionT<T> Lerp(float t, const QuaternionT<T>& q) const;

        QuaternionT<T> Slerp(float t, const QuaternionT<T>& q) const;

        QuaternionT<T> Rotated(const QuaternionT<T>& b) const;

        QuaternionT<T> Multiplied(const QuaternionT<T>& q2) const;

        QuaternionT<T> Scaled(T scale) const;

        T Dot(const QuaternionT<T>& q) const;

        Matrix3 <T> ToMatrix3() const;

        Matrix4 <T> ToMatrix4() const;

        Vector4<T> ToVector() const;

        Vector3<T> ToEulerAngle() const;

        QuaternionT<T> operator-(const QuaternionT<T>& q) const;

        QuaternionT<T> operator+(const QuaternionT<T>& q) const;

        bool operator==(const QuaternionT<T>& q) const;

        bool operator!=(const QuaternionT<T>& q) const;

        void Normalize();

        void Rotate(const QuaternionT<T>& q);

        void Conjugate();

        static QuaternionT<T> CreateFromVectors(const Vector3<T>& v0, const Vector3<T>& v1);

        static QuaternionT<T> AngleAxis(const Vector3<T>& axis, float radians);

        static QuaternionT<T> ToQuaternion(const Matrix4 <T>& matrix);
    };

    template <typename T>
    inline QuaternionT<T>::QuaternionT() : x(0), y(0), z(0), w(1) {
    }

    template <typename T>
    inline QuaternionT<T>::QuaternionT(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {
    }

    template <typename T>
    void QuaternionT<T>::Set(T x, T y, T z, T w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    template <typename T>
    QuaternionT<T> QuaternionT<T>::Clone() const {
        return QuaternionT<T>(x, y, z, w);
    }

    template <typename T>
    inline QuaternionT<T> QuaternionT<T>::Lerp(float t, const QuaternionT<T>& q) const {

        QuaternionT<T> result = QuaternionT<T>(0, 0, 0, 1);
        float dot = w * q.w + x * q.x + y * q.y + z * q.z;
        float deltaTime = 1.f - t;
        if (dot < 0) {
            result.w = deltaTime * w + t * -q.w;
            result.x = deltaTime * x + t * -q.x;
            result.y = deltaTime * y + t * -q.y;
            result.z = deltaTime * z + t * -q.z;
        } else {
            result.w = deltaTime * w + t * q.w;
            result.x = deltaTime * x + t * q.x;
            result.y = deltaTime * y + t * q.y;
            result.z = deltaTime * z + t * q.z;
        }
        result.Normalize();

        return result;
    }

    template <typename T>
    inline QuaternionT<T> QuaternionT<T>::Slerp(float t, const QuaternionT<T>& q) const {

        // Get cosine of angle between quats.
        const float rawCosom = x * q.x + y * q.y + z * q.z + w * q.w;
        // Unaligned quats - compensate, results in taking shorter route.
        const auto cosom = ValueSelect<float>(rawCosom, rawCosom, -rawCosom);

        float scale0, scale1;

        if (cosom < 0.9999f) {
            const float omega = acosf(cosom);
            const float invSin = 1.f / sinf(omega);
            scale0 = sinf((1.f - t) * omega) * invSin;
            scale1 = sinf(t * omega) * invSin;
        }
        else {
            // Use linear interpolation.
            scale0 = 1.0f - t;
            scale1 = t;
        }

        // In keeping with our flipped cosom:
        scale1 = ValueSelect<float>(rawCosom, scale1, -scale1);

        QuaternionT<T> result;

        result.x = scale0 * x + scale1 * q.x;
        result.y = scale0 * y + scale1 * q.y;
        result.z = scale0 * z + scale1 * q.z;
        result.w = scale0 * w + scale1 * q.w;

        return result;
    }

    template <typename T>
    inline QuaternionT<T> QuaternionT<T>::Rotated(const QuaternionT<T>& b) const {
        QuaternionT<T> q;
        q.w = w * b.w - x * b.x - y * b.y - z * b.z;
        q.x = w * b.x + x * b.w + y * b.z - z * b.y;
        q.y = w * b.y + y * b.w + z * b.x - x * b.z;
        q.z = w * b.z + z * b.w + x * b.y - y * b.x;
        q.Normalize();
        return q;
    }

    template <typename T>
    inline QuaternionT<T> QuaternionT<T>::Multiplied(const QuaternionT<T>& q2) const {
        QuaternionT<T> res;
        T A, B, C, D, E, F, G, H;

        A = (w + x) * (q2.w + q2.x);

        B = (z - y) * (q2.y - q2.z);

        C = (w - x) * (q2.y + q2.z);

        D = (y + z) * (q2.w - q2.x);

        E = (x + z) * (q2.x + q2.y);

        F = (x - z) * (q2.x - q2.y);

        G = (w + y) * (q2.w - q2.z);

        H = (w - y) * (q2.w + q2.z);


        res.w = B + (-E - F + G + H) / 2;

        res.x = A - (E + F + G + H) / 2;

        res.y = C + (E - F + G - H) / 2;

        res.z = D + (E - F - G + H) / 2;


        return res;
    }

    template <typename T>
    inline QuaternionT<T> QuaternionT<T>::Scaled(T s) const {
        return QuaternionT<T>(x * s, y * s, z * s, w * s);
    }

    template <typename T>
    inline T QuaternionT<T>::Dot(const QuaternionT<T>& q) const {
        return x * q.x + y * q.y + z * q.z + w * q.w;
    }

    template <typename T>
    inline Matrix3 <T> QuaternionT<T>::ToMatrix3() const {
        QuaternionT<T> q = *this;
        q.Normalize();

        float result[9] = { 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z, 2.0f * q.x * q.y - 2.0f * q.z * q.w,
                            2.0f * q.x * q.z + 2.0f * q.y * q.w,
                            2.0f * q.x * q.y + 2.0f * q.z * q.w, 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z,
                            2.0f * q.y * q.z - 2.0f * q.x * q.w,
                            2.0f * q.x * q.z - 2.0f * q.y * q.w, 2.0f * q.y * q.z + 2.0f * q.x * q.w,
                            1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y };
        return Matrix3<T>(result);
//    const T s = 2;
//    T xs, ys, zs;
//    T wx, wy, wz;
//    T xx, xy, xz;
//    T yy, yz, zz;
//    xs = x * s;  ys = y * s;  zs = z * s;
//    wx = w * xs; wy = w * ys; wz = w * zs;
//    xx = x * xs; xy = x * ys; xz = x * zs;
//    yy = y * ys; yz = y * zs; zz = z * zs;
//    Matrix3<T> m;
//    m.x.x = 1 - (yy + zz); m.y.x = xy - wz;  m.z.x = xz + wy;
//    m.x.y = xy + wz; m.y.y = 1 - (xx + zz); m.z.y = yz - wx;
//    m.x.z = xz - wy; m.y.z = yz + wx;  m.z.z = 1 - (xx + yy);
//    return m;
    }

    template <typename T>
    inline Matrix4 <T> QuaternionT<T>::ToMatrix4() const {
        Matrix4<T> matrix = Matrix4<T>::Identity();
        T xy = x * y;
        T xz = x * z;
        T xw = x * w;
        T yz = y * z;
        T yw = y * w;
        T zw = z * w;
        T xSquared = x * x;
        T ySquared = y * y;
        T zSquared = z * z;
        matrix.x.x = 1 - 2 * (ySquared + zSquared);
        matrix.x.y = 2 * (xy - zw);
        matrix.x.z = 2 * (xz + yw);
        matrix.x.w = 0;
        matrix.y.x = 2 * (xy + zw);
        matrix.y.y = 1 - 2 * (xSquared + zSquared);
        matrix.y.z = 2 * (yz - xw);
        matrix.y.w = 0;
        matrix.z.x = 2 * (xz - yw);
        matrix.z.y = 2 * (yz + xw);
        matrix.z.z = 1 - 2 * (xSquared + ySquared);
        matrix.z.w = 0;
        matrix.w.x = 0;
        matrix.w.y = 0;
        matrix.w.z = 0;
        matrix.w.w = 1;
        return matrix;
    }

    template <typename T>
    inline Vector4<T> QuaternionT<T>::ToVector() const {
        return Vector4<T>(x, y, z, w);
    }

    template <typename T>
    inline Vector3<T> QuaternionT<T>::ToEulerAngle() const {

        // T roll, pitch, yaw;

        // // roll (x-axis rotation)
        // T sinr_cosp = +2.0 * (w * x + y * z);
        // T cosr_cosp = +1.0 - 2.0 * (x * x + y * y);
        // roll = atan2(sinr_cosp, cosr_cosp);

        // // pitch (y-axis rotation)
        // T sinp = +2.0 * (w * y - z * x);
        // if (fabs(sinp) >= 1)
        // 	pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
        // else
        // 	pitch = asin(sinp);

        // // yaw (z-axis rotation)
        // T siny_cosp = +2.0 * (w * z + x * y);
        // T cosy_cosp = +1.0 - 2.0 * (y * y + z * z);
        // yaw = atan2(siny_cosp, cosy_cosp);

        // return Vector3<T>(roll, pitch, yaw);

        T heading;
        T attitude;
        T bank;

        T sqw = w * w;
        T sqx = x * x;
        T sqy = y * y;
        T sqz = z * z;
        T unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
        T test = x * y + z * w;
        if (test > 0.499 * unit) { // singularity at North Pole
            heading = 2 * atan2(x, w);
            attitude = M_PI / 2;
            bank = 0;
            return Vector3<T>(bank, heading, attitude);
        }
        if (test < -0.499 * unit) { // singularity at South Pole
            heading = -2 * atan2(x, w);
            attitude = -M_PI / 2;
            bank = 0;
            return Vector3<T>(bank, heading, attitude);

        }
        heading = atan2(2 * y * w - 2 * x * z, sqx - sqy - sqz + sqw);
        attitude = asin(2 * test / unit);
        bank = atan2(2 * x * w - 2 * y * z, -sqx + sqy - sqz + sqw);

        return Vector3<T>(bank, heading, attitude);
    }

    template <typename T>
    QuaternionT<T> QuaternionT<T>::operator-(const QuaternionT<T>& q) const {
        return QuaternionT<T>(x - q.x, y - q.y, z - q.z, w - q.w);
    }

    template <typename T>
    QuaternionT<T> QuaternionT<T>::operator+(const QuaternionT<T>& q) const {
        return QuaternionT<T>(x + q.x, y + q.y, z + q.z, w + q.w);
    }

    template <typename T>
    bool QuaternionT<T>::operator==(const QuaternionT<T>& q) const {
        return x == q.x && y == q.y && z == q.z && w == q.w;
    }

    template <typename T>
    bool QuaternionT<T>::operator!=(const QuaternionT<T>& q) const {
        return *this != q;
    }

// Compute the quaternion that rotates from a to b, avoiding numerical instability.
// Taken from "The Shortest Arc Quaternion" by Stan Melax in "Game Programming Gems".
    template <typename T>
    inline QuaternionT<T> QuaternionT<T>::CreateFromVectors(const Vector3<T>& v0, const Vector3<T>& v1) {
        if (v0 == -v1)
            return QuaternionT<T>::AngleAxis(vec3(1, 0, 0), Pi);

        Vector3<T> c = v0.Cross(v1);
        T d = v0.Dot(v1);
        T s = std::sqrt((1 + d) * 2);

        QuaternionT<T> q;
        q.x = c.x / s;
        q.y = c.y / s;
        q.z = c.z / s;
        q.w = s / 2.0f;
        return q;
    }

    template <typename T>
    inline QuaternionT<T> QuaternionT<T>::AngleAxis(const Vector3<T>& axis, float radians) {
        QuaternionT<T> q;
        q.w = std::cos(radians / 2);
        q.x = q.y = q.z = std::sin(radians / 2);
        q.x *= axis.x;
        q.y *= axis.y;
        q.z *= axis.z;
        return q;

        // T cy = cos(axis.z * radians * 0.5);
        // T sy = sin(axis.z * radians * 0.5);
        // T cp = cos(axis.y * radians * 0.5);
        // T sp = sin(axis.y * radians * 0.5);
        // T cr = cos(axis.x * radians * 0.5);
        // T sr = sin(axis.x * radians * 0.5);

        // QuaternionT<T> q;
        // q.w = cy * cp * cr + sy * sp * sr;
        // q.x = cy * cp * sr - sy * sp * cr;
        // q.y = sy * cp * sr + cy * sp * cr;
        // q.z = sy * cp * cr - cy * sp * sr;
        // return q;
    }

    template <typename T>
    inline void QuaternionT<T>::Normalize() {
        *this = Scaled(1 / std::sqrt(Dot(*this)));
    }

    template <typename T>
    inline void QuaternionT<T>::Rotate(const QuaternionT<T>& q2) {
        QuaternionT<T> q;
        QuaternionT<T>& q1 = *this;

        q.w = w * q2.w - x * q2.x - y * q2.y - z * q2.z;
        q.x = w * q2.x + x * q2.w + y * q2.z - z * q2.y;
        q.y = w * q2.y + y * q2.w + z * q2.x - x * q2.z;
        q.z = w * q2.z + z * q2.w + x * q2.y - y * q2.x;

        q.Normalize();
        *this = q;
    }

    template <typename T>
    inline void QuaternionT<T>::Conjugate() {
        QuaternionT<T> q;
        QuaternionT<T>& q1 = *this;

        q.w = w;
        q.x = -x;
        q.y = -y;
        q.z = -z;

        *this = q;
    }

    template <typename T>
    QuaternionT<T> QuaternionT<T>::ToQuaternion(const Matrix4 <T>& matrix) {
        QuaternionT<T> q;
        q.w = sqrt(fmax(0, 1 + matrix.x.x + matrix.y.y + matrix.z.z)) / 2;
        q.x = sqrt(fmax(0, 1 + matrix.x.x - matrix.y.y - matrix.z.z)) / 2;
        q.y = sqrt(fmax(0, 1 - matrix.x.x + matrix.y.y - matrix.z.z)) / 2;
        q.z = sqrt(fmax(0, 1 - matrix.x.x - matrix.y.y + matrix.z.z)) / 2;
        q.x *= sign<T>(q.x * (matrix.z.y - matrix.y.z));
        q.y *= sign<T>(q.y * (matrix.x.z - matrix.z.x));
        q.z *= sign<T>(q.z * (matrix.y.x - matrix.x.y));
        return q;
    }

    typedef QuaternionT<float> Quaternion;
}