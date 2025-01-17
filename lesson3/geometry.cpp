#include "geometry.h"
Vec3f cross(const Vec3f &v1, const Vec3f &v2)
{
    return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
}