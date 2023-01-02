#pragma once
#include "Header.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>

namespace Zero
{
namespace ZMath
{
    using namespace glm;
    
    using FColor4 = glm::vec4;

    using FColor3 = glm::vec3;
    
    using FRotation = glm::vec3;

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


	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		// Constant buffers must be a multiple of the minimum hardware
		// allocation size (usually 256 bytes).  So round up to nearest
		// multiple of 256.  We do this by adding 255 and then masking off
		// the lower 2 bytes which store all bits < 256.
		// Example: Suppose byteSize = 300.
		// (300 + 255) & ~255
		// 555 & ~255
		// 0x022B & ~0x00ff
		// 0x022B & 0xff00
		// 0x0200
		// 512
		return (byteSize + 255) & ~255;
	}

	static bool DecomposeTransform(const glm::mat4& Transform, glm::vec3& Translation, glm::vec3& Rotation, glm::vec3& Scale)
	{
		// From glm::decompose in matrix_decompose.inl

		using namespace glm;
		using T = float;

		mat4 LocalMatrix(Transform);

		// Normalize the matrix.
		if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		Translation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

        vec3 Row[3];// , Pdum3;

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		Scale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		Scale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		Scale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

		Rotation.y = asin(-Row[0][2]);
		if (cos(Rotation.y) != 0) {
			Rotation.x = atan2(Row[1][2], Row[2][2]);
			Rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else {
			Rotation.x = atan2(-Row[2][0], Row[1][1]);
			Rotation.z = 0;
		}

		return true;
	}
}
static std::ostream& operator<<(std::ostream& os, ZMath::vec2& pt)
{
    os << std::format("[{0}, {1}]", pt.x, pt.y);
    return os;
}
static std::ostream& operator<<(std::ostream& os, ZMath::vec3& pt)
{
	os << std::format("[{0}, {1}, {2}]", pt.x, pt.y, pt.z);
	return os;
}
static std::ostream& operator<<(std::ostream& os, ZMath::vec4& pt)
{
	os << std::format("[{0}, {1}, {2}, {3}]", pt.x, pt.y, pt.z, pt.w);
	return os;
}
static std::ostream& operator<<(std::ostream& os, ZMath::ivec2& pt)
{
    os << std::format("[{0}, {1}]", pt.x, pt.y);
    return os;
}
static std::ostream& operator<<(std::ostream& os, ZMath::ivec3& pt)
{
    os << std::format("[{0}, {1}, {2}]", pt.x, pt.y, pt.z);
    return os;
}
static std::ostream& operator<<(std::ostream& os, ZMath::ivec4& pt)
{
    os << std::format("[{0}, {1}, {2}, {3}]", pt.x, pt.y, pt.z, pt.w);
    return os;
}
static std::ostream& operator<<(std::ostream& os, ZMath::mat4& mat)
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            os << mat[i][j] << " ";
        }
        os << "\n";
    }
    return os;
}
}