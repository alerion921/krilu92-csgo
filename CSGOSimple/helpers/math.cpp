
#include "Math.hpp"

namespace Math
{
    void Math::angleToForwardVector(const QAngle& angles, Vector& forward) 
    {
        float xRad = DEG2RAD(angles.pitch);
        float yRad = DEG2RAD(angles.yaw);
        float cosX = (float)cos(xRad);
        float sinX = (float)sin(xRad);
        float cosY = (float)cos(yRad);
        float sinY = (float)sin(yRad);
        forward.x = cosX * cosY;
        forward.y = cosX * sinY;
        forward.z = -sinX;
    }

    void Math::AngleVectors1234(const Vector& angles, Vector* forward)
    {
        float sr, sp, sy, cr, cp, cy;
        SinCos(DEG2RAD(angles[1]), &sy, &cy);
        SinCos(DEG2RAD(angles[0]), &sp, &cp);
        SinCos(DEG2RAD(angles[2]), &sr, &cr);

        if (forward)
        {
            forward->x = cp * cy;
            forward->y = cp * sy;
            forward->z = -sp;
        }
    }

    void VectorAngles1(QAngle& angles, Vector forward)
    {
        //Assert(s_bMathlibInitialized);
        float yaw, pitch;

        if (forward[1] == 0 && forward[0] == 0)
        {
            yaw = 0;
            if (forward[2] > 0)
                pitch = 270;
            else
                pitch = 90;
        }
        else
        {
            yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
            if (yaw < 0)
                yaw += 360;

            float tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
            pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
            if (pitch < 0)
                pitch += 360;
        }

        angles[0] = pitch;
        angles[1] = yaw;
        angles[2] = 0;
    }
    //--------------------------------------------------------------------------------
    float VectorDistance(const Vector& v1, const Vector& v2)
    {
        return FASTSQRT(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
    }
    //--------------------------------------------------------------------------------
    void NormalizeAngles(QAngle& angles)
    {
        for (auto i = 0; i < 3; i++)
        {
            while (angles[i] < -180.0f)
            {
                angles[i] += 360.0f;
            }
            while (angles[i] > 180.0f)
            {
                angles[i] -= 360.0f;
            }
        }
    }
    void NormalizePitch(float& pitch)
    {
        while (pitch < -180.0f)
        {
            pitch += 360.0f;
        }
        while (pitch > 180.0f)
        {
            pitch -= 360.0f;
        }
    }
    int RandomInt(int min, int max)
    {
        return rand() % (max - min + 1) + min;
    }
    QAngle CalcAngle(const Vector& src, const Vector& dst)
    {
        /*
        QAngle vAngle;
        Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
        double hyp = sqrt(delta.x*delta.x + delta.y*delta.y);

        vAngle.pitch = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
        vAngle.yaw = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
        vAngle.roll = 0.0f;

        if (delta.x >= 0.0) vAngle.yaw += 180.0f;
        */
        //QAngle vAngle;
        //Math::VectorAngles(dst - src, vAngle);
        //return vAngle;
        QAngle vAngle;
        Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
        double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

        vAngle.pitch = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
        vAngle.yaw = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
        vAngle.roll = 0.0f;

        if (delta.x >= 0.0)
        {
            vAngle.yaw += 180.0f;
        }

        return vAngle;
    }
    Vector CrossProduct2(const Vector& a, const Vector& b)
    {
        return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }
    void VectorAngles(const Vector& forward, Vector& up, QAngle& angles)
    {
        Vector left = CrossProduct2(up, forward);
        left.NormalizeInPlace();

        float forwardDist = forward.Length2D();

        if (forwardDist > 0.001f)
        {
            angles.pitch = atan2f(-forward.z, forwardDist) * 180 / PI_F;
            angles.yaw = atan2f(forward.y, forward.x) * 180 / PI_F;

            float upZ = (left.y * forward.x) - (left.x * forward.y);
            angles.roll = atan2f(left.z, upZ) * 180 / PI_F;
        }
        else
        {
            angles.pitch = atan2f(-forward.z, forwardDist) * 180 / PI_F;
            angles.yaw = atan2f(-left.x, left.y) * 180 / PI_F;
            angles.roll = 0;
        }
    }
    float RandomFloat(float min, float max)
    {
        static auto ranFloat = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandleW(L"vstdlib.dll"), "RandomFloat"));
        if (ranFloat)
        {
            return ranFloat(min, max);
        }
        else
        {
            return 0.f;
        }
    }
    //--------------------------------------------------------------------------------
    float GetFOV(const QAngle& viewAngle, const QAngle& aimAngle)
    {
        Vector ang, aim;

        AngleVectors(viewAngle, aim);
        AngleVectors(aimAngle, ang);

        return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
    }
    void AngleVector(const Vector& angles, Vector& forward)
    {
        Assert(s_bMathlibInitialized);
        Assert(forward);

        float sp, sy, cp, cy;

        Math::SinCos(DEG2RAD(angles.y), &sy, &cy);
        Math::SinCos(DEG2RAD(angles.x), &sp, &cp);

        forward.x = cp * cy;
        forward.y = cp * sy;
        forward.z = -sp;
    }
    //--------------------------------------------------------------------------------
    void ClampAngles(QAngle& angles)
    {
        if (angles.pitch > 89.0f)
        {
            angles.pitch = 89.0f;
        }
        else if (angles.pitch < -89.0f)
        {
            angles.pitch = -89.0f;
        }

        if (angles.yaw > 180.0f)
        {
            angles.yaw = 180.0f;
        }
        else if (angles.yaw < -180.0f)
        {
            angles.yaw = -180.0f;
        }

        angles.roll = 0;
    }
    //--------------------------------------------------------------------------------
    void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
    {
        //auto t = in2[0];
        out[0] = in1.Dot(in2[0]) + in2[0][3];
        out[1] = in1.Dot(in2[1]) + in2[1][3];
        out[2] = in1.Dot(in2[2]) + in2[2][3];
    }
    //--------------------------------------------------------------------------------
    void AngleVectors(const QAngle& angles, Vector& forward)
    {
        float	sp, sy, cp, cy;

        DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
        DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));

        forward.x = cp * cy;
        forward.y = cp * sy;
        forward.z = -sp;
    }

    void AngleVectorsAuto(const QAngle& angles, Vector* forward)
    {
        Assert(s_bMathlibInitialized);
        Assert(forward);

        float	sp, sy, cp, cy;

        sy = sin(DEG2RAD(angles[1]));
        cy = cos(DEG2RAD(angles[1]));

        sp = sin(DEG2RAD(angles[0]));
        cp = cos(DEG2RAD(angles[0]));

        forward->x = cp * cy;
        forward->y = cp * sy;
        forward->z = -sp;
    }

    void AngleVectors1337(const QAngle& angles, Vector* forward)
    {
        Assert(s_bMathlibInitialized);
        Assert(forward);

        float	sp, sy, cp, cy;

        sy = sin(DEG2RAD(angles[1]));
        cy = cos(DEG2RAD(angles[1]));

        sp = sin(DEG2RAD(angles[0]));
        cp = cos(DEG2RAD(angles[0]));

        forward->x = cp * cy;
        forward->y = cp * sy;
        forward->z = -sp;
    }

    void calculate_angle1337(Vector src, Vector dst, QAngle& angles)
    {
        Vector delta = src - dst;
        double hyp = delta.Length2D();
        angles.yaw = (atan(delta.y / delta.x) * 57.295779513082f);
        angles.pitch = (atan(delta.z / hyp) * 57.295779513082f);
        angles[2] = 0.0f;
        if (delta.x >= 0.0) angles.roll += 180.0f;
    }

    //--------------------------------------------------------------------------------
    void AngleVectors(const QAngle& angles, Vector& forward, Vector& right, Vector& up)
    {
        float sr, sp, sy, cr, cp, cy;

        DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
        DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
        DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angles[2]));

        forward.x = (cp * cy);
        forward.y = (cp * sy);
        forward.z = (-sp);
        right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
        right.y = (-1 * sr * sp * sy + -1 * cr * cy);
        right.z = (-1 * sr * cp);
        up.x = (cr * sp * cy + -sr * -sy);
        up.y = (cr * sp * sy + -sr * cy);
        up.z = (cr * cp);
    }
    //--------------------------------------------------------------------------------
    void VectorAngles(const Vector& forward, QAngle& angles)
    {
        float	tmp, yaw, pitch;

        if (forward[1] == 0 && forward[0] == 0)
        {
            yaw = 0;
            if (forward[2] > 0)
            {
                pitch = 270;
            }
            else
            {
                pitch = 90;
            }
        }
        else
        {
            yaw = (atan2(forward[1], forward[0]) * 180 / DirectX::XM_PI);
            if (yaw < 0)
            {
                yaw += 360;
            }

            tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
            pitch = (atan2(-forward[2], tmp) * 180 / DirectX::XM_PI);
            if (pitch < 0)
            {
                pitch += 360;
            }
        }

        angles[0] = pitch;
        angles[1] = yaw;
        angles[2] = 0;
    }
    //--------------------------------------------------------------------------------
    static bool screen_transform(const Vector& in, Vector& out)
    {
        static auto& w2sMatrix = g_EngineClient->WorldToScreenMatrix();

        out.x = w2sMatrix.m[0][0] * in.x + w2sMatrix.m[0][1] * in.y + w2sMatrix.m[0][2] * in.z + w2sMatrix.m[0][3];
        out.y = w2sMatrix.m[1][0] * in.x + w2sMatrix.m[1][1] * in.y + w2sMatrix.m[1][2] * in.z + w2sMatrix.m[1][3];
        out.z = 0.0f;

        float w = w2sMatrix.m[3][0] * in.x + w2sMatrix.m[3][1] * in.y + w2sMatrix.m[3][2] * in.z + w2sMatrix.m[3][3];

        if (w < 0.001f)
        {
            out.x *= 100000;
            out.y *= 100000;
            return false;
        }

        out.x /= w;
        out.y /= w;

        return true;
    }
    //--------------------------------------------------------------------------------
    bool WorldToScreen(const Vector& in, Vector& out)
    {
        if (screen_transform(in, out))
        {
            int w, h;
            g_EngineClient->GetScreenSize(w, h);

            out.x = (w / 2.0f) + (out.x * w) / 2.0f;
            out.y = (h / 2.0f) - (out.y * h) / 2.0f;

            return true;
        }
        return false;
    }
    //--------------------------------------------------------------------------------
}