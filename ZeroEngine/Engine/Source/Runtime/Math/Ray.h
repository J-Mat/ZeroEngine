#pragma once

#include "AABB.h"
#include "ZMath.h"

namespace Zero
{
    namespace ZMath
    {
        struct FRay
        {
            ZMath::vec3 Origin, Direction;

            FRay(const ZMath::vec3& _Origin, const ZMath::vec3& _Direction)
            {
                Origin = _Origin;
                Direction = _Direction;
            }

            static FRay Zero()
            {
                return { {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f} };
            }

            bool IntersectsAABB(const FAABB& aabb, float& t) const
            {
                ZMath::vec3 dirfrac;
                // r.dir is unit direction vector of ray
                dirfrac.x = 1.0f / Direction.x;
                dirfrac.y = 1.0f / Direction.y;
                dirfrac.z = 1.0f / Direction.z;
                // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
                // r.org is origin of ray
                const ZMath::vec3& lb = aabb.Min;
                const ZMath::vec3& rt = aabb.Max;
                float t1 = (lb.x - Origin.x) * dirfrac.x;
                float t2 = (rt.x - Origin.x) * dirfrac.x;
                float t3 = (lb.y - Origin.y) * dirfrac.y;
                float t4 = (rt.y - Origin.y) * dirfrac.y;
                float t5 = (lb.z - Origin.z) * dirfrac.z;
                float t6 = (rt.z - Origin.z) * dirfrac.z;

                float tmin = ZMath::max(ZMath::max(ZMath::min(t1, t2), ZMath::min(t3, t4)), ZMath::min(t5, t6));
                float tmax = ZMath::min(ZMath::min(ZMath::max(t1, t2), ZMath::max(t3, t4)), ZMath::max(t5, t6));

                // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
                if (tmax < 0)
                {
                    t = tmax;
                    return false;
                }

                // if tmin > tmax, ray doesn't intersect AABB
                if (tmin > tmax)
                {
                    t = tmax;
                    return false;
                }

                t = tmin;
                return true;
            }

            bool IntersectsTriangle(const ZMath::vec3& a, const ZMath::vec3& b, const ZMath::vec3& c, float& t) const
            {
                const ZMath::vec3 E1 = b - a;
                const ZMath::vec3 E2 = c - a;
                const ZMath::vec3 N = cross(E1, E2);
                const float det = -ZMath::dot(Direction, N);
                const float invdet = 1.f / det;
                const ZMath::vec3 AO = Origin - a;
                const ZMath::vec3 DAO = ZMath::cross(AO, Direction);
                const float u = ZMath::dot(E2, DAO) * invdet;
                const float v = -ZMath::dot(E1, DAO) * invdet;
                t = ZMath::dot(AO, N) * invdet;
                return (det >= 1e-6f && t >= 0.0f && u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f);
            }
        };
    }
}