#pragma once 
#include "PCH.h" 
#include "Log.h"


#define EDITOR_MODE 1

#define _KB(x) (x * 1024)
#define _MB(x) (x * 1024 * 1024)

#define _64KB _KB(64)
#define _1MB _MB(1)
#define _2MB _MB(2)
#define _4MB _MB(4)
#define _8MB _MB(8)
#define _16MB _MB(16)
#define _32MB _MB(32)
#define _64MB _MB(64)
#define _128MB _MB(128)
#define _256MB _MB(256)


#define BIT(x) (1 << x)
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
#define TO_VOIDPTR_FROM_DATA(Value) ((void*)&Value)
#define TO_VOID_PTR_FROM_PTR(Ptr) ((void*)Ptr)


#define CUBEMAP_TEXTURE_CNT 6
#define DEFAULT_TEXTURE_HANDLE "default"

#define ZERO_INVALID_ID -1

namespace Zero
{
// Set the name of an std::thread.
// Useful for debugging.
	const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack( push, 8 )
	typedef struct tagTHREADNAME_INFO
	{
		DWORD  dwType;      // Must be 0x1000.
		LPCSTR szName;      // Pointer to name (in user addr space).
		DWORD  dwThreadID;  // Thread ID (-1=caller thread).
		DWORD  dwFlags;     // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack( pop )


	namespace Utils
	{
		inline void SetThreadName(std::thread& thread, const char* threadName)
		{
			THREADNAME_INFO info;
			info.dwType = 0x1000;
			info.szName = threadName;
			info.dwThreadID = ::GetThreadId(reinterpret_cast<HANDLE>(thread.native_handle()));
			info.dwFlags = 0;

			__try
			{
				::RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}
	}

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Weak = std::weak_ptr<T>;

	struct FTextureHandleType
	{
		std::string TextureName = "";
		uint32_t ID = ZERO_INVALID_ID;
		FTextureHandleType() = default;
		FTextureHandleType(std::string _TextureName)
			: TextureName(_TextureName)
		{}
		FTextureHandleType(std::string _TextureName, uint32_t _ID)
			: TextureName(_TextureName)
			, ID(_ID)
		{}
		bool IsNull()
		{
			return ID == -1 || TextureName == "";
		}
		auto operator<=>(FTextureHandleType const&) const = default;
	};
	using FTextureHandle = FTextureHandleType;

	using FJsonObj = nlohmann::json;
	using FMaterialHandle = std::string;
	using FAssetHandle = Utils::Guid;
	using FShaderFileHandle = std::string;
	using FCommandListHandle = uint32_t;
	
	struct FFloatSlider
	{
		float Value = 0.0f;
		float Min = 0.0f;
		float Max = 1.0f;
		bool bEnableEdit = false;
	};

	namespace Utils
	{
		const int32_t InvalidIndex = -1;
		const std::map<std::string, size_t> CodeReflectionTypes =
		{
			{"bool", sizeof(bool)},
			{"int", sizeof(int)},
			{"int32_t", sizeof(int32_t)},
			{"std::string", sizeof(std::string)},
			{"string", sizeof(std::string)},
			{"uint32_t", sizeof(uint32_t)},
			{"int32_t", sizeof(int32_t)},
			{"ZMath::vec3", sizeof(ZMath::vec3)},
			{"ZMath::vec4", sizeof(ZMath::vec4)},
			{"ZMath::FColor4", sizeof(ZMath::FColor4)},
			{"ZMath::FColor3", sizeof(ZMath::FColor3)},
			{"ZMath::FRotation", sizeof(ZMath::FRotation)},
			{"float", sizeof(float)},
			{"double", sizeof(double)},
			{"FTextureHandle", sizeof(FTextureHandle) },
			{"FMaterialHandle", sizeof(FMaterialHandle)},
			{"std::filesystem::path", sizeof(std::filesystem::path)},
			{"FAssetHandle", sizeof(FAssetHandle)},
			{"FShaderFileHandle", sizeof(FShaderFileHandle)},
			{"FFloatSlider", sizeof(FFloatSlider)}
		};
	}

	template <typename T>
	class FIndexGetter {
	public:
		static uint32_t Get() {
			static uint32_t ID = m_CurIdx++;
			return ID;
		}

	private:
		inline static uint32_t m_CurIdx = 0;
	};


	template<typename T>
	concept Releasable = requires (T t)
	{
		{t.Release()};
	};

	struct ReleaseDeleter
	{
		template<Releasable T>
		void operator()(T* allocation)
		{
			allocation->Release();
		}
	};

	template<Releasable T>
	using ReleasablePtr = std::unique_ptr<T, ReleaseDeleter>;
}

namespace std
{
	template <> struct hash<Zero::FTextureHandle>
	{
		size_t operator()(Zero::FTextureHandle const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
}