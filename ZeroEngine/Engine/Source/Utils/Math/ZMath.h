#pragma once
#include "Header.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Zero
{
namespace ZMath
{
    using namespace glm;
    
    using FColor = glm::vec4;
    
    struct FEulerAngle
    {
        FEulerAngle() = default;
        float Pitch = 0.0f;
        float Yaw = 0.0f;
        float Roll = 0.0f;
    };
    
    constexpr float PI = 3.1415926535897932384626433832795f;
    constexpr float _2PI = 2.0f * PI;
    constexpr vec3 DegreeVec(vec3 Rotation)
    {
        return vec3(degrees(Rotation.x), degrees(Rotation.y), degrees(Rotation.z));
    }
    
    constexpr glm::vec3 World_Up = { 0.0f, 1.0f, 0.0f };
    constexpr glm::vec3 World_Right = { 1.0f, 0.0f, 0.0f };
    constexpr glm::vec3 World_Forward = { 0.0f, 0.0f, 1.0f };
    

    // Convert degrees to radians.
    constexpr float Radians(const float degrees)
    {
        return degrees * (PI / 180.0f);
    }

    template<typename T>
    inline T Deadzone(T val, T deadzone)
    {
        if (std::abs(val) < deadzone)
        {
            return T(0);
        }

        return val;
    }

    // Normalize a value in the range [min - max]
    template<typename T, typename U>
    inline T NormalizeRange(U x, U min, U max)
    {
        return T(x - min) / T(max - min);
    }

    // Shift and bias a value into another range.
    template<typename T, typename U>
    inline T ShiftBias(U x, U shift, U bias)
    {
        return T(x * bias) + T(shift);
    }

    template <typename T>
    inline T AlignUpWithMask(T value, size_t mask)
    {
        return (T)(((size_t)value + mask) & ~mask);
    }

    template <typename T>
    inline T AlignDownWithMask(T value, size_t mask)
    {
        return (T)((size_t)value & ~mask);
    }

    template <typename T>
    inline T AlignUp(T value, size_t alignment)
    {
        return AlignUpWithMask(value, alignment - 1);
    }

    template <typename T>
    inline T AlignDown(T value, size_t alignment)
    {
        return AlignDownWithMask(value, alignment - 1);
    }

    template <typename T>
    inline bool IsAligned(T value, size_t alignment)
    {
        return 0 == ((size_t)value & (alignment - 1));
    }

    template <typename T>
    inline T DivideByMultiple(T value, size_t alignment)
    {
        return (T)((value + alignment - 1) / alignment);
    }

    inline uint32_t NextHighestPow2(uint32_t v)
    {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;

        return v;
    }

    inline uint64_t NextHighestPow2(uint64_t v)
    {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v |= v >> 32;
        v++;

        return v;
    }
}
}