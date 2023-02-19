#pragma once

#include "AABB.h"
#include "ZMath.h"

namespace Zero
{
    namespace ZMath
    {
        class FRay
        {
        public:
            ZMath::vec3 m_Origin, m_Direction;

            FRay(const ZMath::vec3& _Origin, const ZMath::vec3& _Direction)
            {
                m_Origin = _Origin;
                m_Direction = _Direction;
            }
            
            ZMath::vec3 GetPos(float Distance)
            {
                return m_Origin + m_Direction * Distance;
            }

            static FRay Zero()
            {
                return { {0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f} };
            }

            bool IntersectsAABB(const FAABB& AABB, float& T) const
            {
                ZMath::vec3 DirFrac;
                // r.dir is unit direction vector of ray
                DirFrac.x = 1.0f / m_Direction.x;
                DirFrac.y = 1.0f / m_Direction.y;
                DirFrac.z = 1.0f / m_Direction.z;
                // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
                // r.org is origin of ray
                const ZMath::vec3& lb = AABB.Min;
                const ZMath::vec3& rt = AABB.Max;
                float t1 = (lb.x - m_Origin.x) * DirFrac.x;
                float t2 = (rt.x - m_Origin.x) * DirFrac.x;
                float t3 = (lb.y - m_Origin.y) * DirFrac.y;
                float t4 = (rt.y - m_Origin.y) * DirFrac.y;
                float t5 = (lb.z - m_Origin.z) * DirFrac.z;
                float t6 = (rt.z - m_Origin.z) * DirFrac.z;

                float MinT = ZMath::max(ZMath::max(ZMath::min(t1, t2), ZMath::min(t3, t4)), ZMath::min(t5, t6));
                float MaxT = ZMath::min(ZMath::min(ZMath::max(t1, t2), ZMath::max(t3, t4)), ZMath::max(t5, t6));

                // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
                if (MaxT < 0)
                {
                    T = MaxT;
                    return false;
                }

                // if tmin > tmax, ray doesn't intersect AABB
                if (MinT > MaxT)
                {
                    T = MaxT;
                    return false;
                }

                T = MinT;
                return true;
            }

            bool IntersectsTriangle(const ZMath::vec3& A, const ZMath::vec3& B, const ZMath::vec3& C, float& T) const
            {
                const ZMath::vec3 E1 = B - A;
                const ZMath::vec3 E2 = C - A;
                const ZMath::vec3 N = cross(E1, E2);
                const float Det = -ZMath::dot(m_Direction, N);
                const float InvDet = 1.f / Det;
                const ZMath::vec3 AO = m_Origin - A;
                const ZMath::vec3 DAO = ZMath::cross(AO, m_Direction);
                const float U = ZMath::dot(E2, DAO) * InvDet;
                const float V = -ZMath::dot(E1, DAO) * InvDet;
                T = ZMath::dot(AO, N) * InvDet;
                return (Det >= 1e-6f && T >= 0.0f && U >= 0.0f && V >= 0.0f && (U + V) <= 1.0f);
            }
            
            FRay TransformRay (const ZMath::mat4& Transform) const
            {
                // 影响位移，后面补1
                ZMath::vec3 Origin = Transform * ZMath::vec4(m_Origin, 1.0f);
                // 不影响位移，后面补0
                ZMath::vec3 Direciton = Transform * ZMath::vec4(m_Direction, 0.0f);
                
                return { Origin, Direciton };
            }
        };

    }
}