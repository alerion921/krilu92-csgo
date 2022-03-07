#pragma once

#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

#define Assert(_exp) ((void)0)

#define CHECK_VALID( _v ) 0

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.
#define deeg2raad( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )

class Vector
{
public:
    Vector(void)
    {
        Invalidate();
    }
    Vector(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }

    Vector(const float* clr)
    {
        x = clr[0];
        y = clr[1];
        z = clr[2];
    }

    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
    {
        x = ix; y = iy; z = iz;
    }

    Vector Direction()
    {
        return Vector(cos(y * (float)M_PI / 180.0f) * cos(x * (float)M_PI / 180.0f), sin(y * (float)M_PI / 180.0f) * cos(x * (float)M_PI / 180.0f), sin(-x * (float)M_PI / 180.0f)).Normalized();
    }

    inline void VectorSubtract(const Vector& a, const Vector& b, Vector& c)
    {
        CHECK_VALID(a);
        CHECK_VALID(b);
        c.x = a.x - b.x;
        c.y = a.y - b.y;
        c.z = a.z - b.z;
    }

    inline void VectorMultiply(const Vector& a, float b, Vector& c)
    {
        CHECK_VALID(a);
        Assert(IsFinite(b));
        c.x = a.x * b;
        c.y = a.y * b;
        c.z = a.z * b;
    }

    Vector Vector::operator+(float fl) const
    {
        return Vector(x + fl, y + fl, z + fl);
    }
    float Vector::NormalizeInPlace()
    {
        Vector& v = *this;

        float iradius = 1.f / (this->Length() + 1.192092896e-07F); //FLT_EPSILON

        v.x *= iradius;
        v.y *= iradius;
        v.z *= iradius;
        return 1;
    }

    Vector Vector::operator-(float fl) const
    {
        return Vector(x - fl, y - fl, z - fl);
    }
    void VectorCrossProduct(const Vector& a, const Vector& b, Vector& result)
    {
        result.x = a.y * b.z - a.z * b.y;
        result.y = a.z * b.x - a.x * b.z;
        result.z = a.x * b.y - a.y * b.x;
    }
    Vector Cross(const Vector& vOther)
    {
        Vector res;
        VectorCrossProduct(*this, vOther, res);
        return res;
    }

    bool IsValid() const
    {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }
    void Invalidate()
    {
        x = y = z = std::numeric_limits<float>::infinity();
    }

    float& operator[](int i)
    {
        return ((float*)this)[i];
    }
    float operator[](int i) const
    {
        return ((float*)this)[i];
    }

    void Zero()
    {
        x = y = z = 0.0f;
    }

    bool operator==(const Vector& src) const
    {
        return (src.x == x) && (src.y == y) && (src.z == z);
    }
    bool operator!=(const Vector& src) const
    {
        return (src.x != x) || (src.y != y) || (src.z != z);
    }

    Vector& operator+=(const Vector& v)
    {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    Vector& operator-=(const Vector& v)
    {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    Vector& operator*=(float fl)
    {
        x *= fl;
        y *= fl;
        z *= fl;
        return *this;
    }
    Vector& operator*=(const Vector& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    Vector& operator/=(const Vector& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }
    Vector& operator+=(float fl)
    {
        x += fl;
        y += fl;
        z += fl;
        return *this;
    }
    Vector& operator/=(float fl)
    {
        x /= fl;
        y /= fl;
        z /= fl;
        return *this;
    }
    Vector& operator-=(float fl)
    {
        x -= fl;
        y -= fl;
        z -= fl;
        return *this;
    }

    Vector Normalized() const
    {
        Vector res = *this;
        float l = res.Length();
        if (l != 0.0f) {
            res /= l;
        }
        else {
            res.x = res.y = res.z = 0.0f;
        }
        return res;
    }

    float DistTo(const Vector& vOther) const
    {
        Vector delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.Length();
    }
    float DistToSqr(const Vector& vOther) const
    {
        Vector delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.LengthSqr();
    }
    float Dot(const Vector& vOther) const
    {
        return (x * vOther.x + y * vOther.y + z * vOther.z);
    }
    float Length() const
    {
        return sqrt(x * x + y * y + z * z);
    }
    float LengthSqr(void) const
    {
        return (x * x + y * y + z * z);
    }
    float Length2D() const
    {
        return sqrt(x * x + y * y);
    }

    Vector& operator=(const Vector& vOther)
    {
        x = vOther.x; y = vOther.y; z = vOther.z;
        return *this;
    }

    Vector Vector::operator-(void) const
    {
        return Vector(-x, -y, -z);
    }
    Vector Vector::operator+(const Vector& v) const
    {
        return Vector(x + v.x, y + v.y, z + v.z);
    }
    Vector Vector::operator-(const Vector& v) const
    {
        return Vector(x - v.x, y - v.y, z - v.z);
    }
    Vector Vector::operator*(float fl) const
    {
        return Vector(x * fl, y * fl, z * fl);
    }
    Vector Vector::operator*(const Vector& v) const
    {
        return Vector(x * v.x, y * v.y, z * v.z);
    }
    Vector Vector::operator/(float fl) const
    {
        return Vector(x / fl, y / fl, z / fl);
    }
    Vector Vector::operator/(const Vector& v) const
    {
        return Vector(x / v.x, y / v.y, z / v.z);
    }

    inline void SinCos(const float& r, float& s, float& c)
    {
        s = sin(r);
        c = cos(r);
    }

    inline void Vector::Rotate2D(const float& f)
    {
        float _x, _y;

        float s, c;

        SinCos(deeg2raad(f), s, c);

        _x = x;
        _y = y;

        x = (_x * c) - (_y * s);
        y = (_x * s) + (_y * c);
    }

    float x, y, z;
};

inline Vector operator*(float lhs, const Vector& rhs)
{
    return rhs * lhs;
}
inline Vector operator/(float lhs, const Vector& rhs)
{
    return rhs / lhs;
}

class __declspec(align(16)) VectorAligned : public Vector
{
public:
    inline VectorAligned(void) {};
    inline VectorAligned(float X, float Y, float Z)
    {
        Init(X, Y, Z);
    }

public:
    explicit VectorAligned(const Vector& vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
    }

    VectorAligned& operator=(const Vector& vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }

    VectorAligned& operator=(const VectorAligned& vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }

    float w;
};