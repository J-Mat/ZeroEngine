#include "DX12ShaderCompiler.h"
//#include <d3dcompiler.h> 
#include <wrl.h>
#include "ZConfig.h"
#include "FileUtil.h"
#include "dxc/inc/d3d12shader.h"
#include "dxc/inc/dxcapi.h"

namespace Zero
{
	namespace
	{
		ComPtr<IDxcLibrary> DxcLibrary = nullptr;
		ComPtr<IDxcCompiler3> DxcCompiler = nullptr;
		ComPtr<IDxcUtils> DxcUtils = nullptr;
		ComPtr<IDxcIncludeHandler> DxcIncludeHandler = nullptr;
	}
	class FCustomIncludeHandler : public IDxcIncludeHandler
	{
	public:
		FCustomIncludeHandler() {}

		HRESULT STDMETHODCALLTYPE LoadSource(_In_ LPCWSTR pFilename, _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource) override
		{
			ComPtr<IDxcBlobEncoding> encoding;
			std::string IncludeFile = FileUtil::NormalizePath(Utils::WString2String(pFilename));
			if (!FileUtil::FileExists(IncludeFile))
			{
				*ppIncludeSource = nullptr;
				return E_FAIL;
			}

			bool already_included = false;
			for (auto const& included_file : IncludeFiles)
			{
				if (IncludeFile == included_file)
				{
					already_included = true;
					break;
				}
			}

			if (already_included)
			{
				static const char nullStr[] = " ";
				DxcUtils->CreateBlob(nullStr, ARRAYSIZE(nullStr), CP_UTF8, encoding.GetAddressOf());
				*ppIncludeSource = encoding.Detach();
				return S_OK;
			}

			std::wstring winclude_file = Utils::String2WString(IncludeFile);
			HRESULT hr = DxcUtils->LoadFile(winclude_file.c_str(), nullptr, encoding.GetAddressOf());
			if (SUCCEEDED(hr))
			{
				IncludeFiles.insert(IncludeFile);
				*ppIncludeSource = encoding.Detach();
				return S_OK;
			}
			else *ppIncludeSource = nullptr;
			return E_FAIL;
		}
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override
		{
			return DxcIncludeHandler->QueryInterface(riid, ppvObject);
		}

		ULONG STDMETHODCALLTYPE AddRef(void) override { return 1; }
		ULONG STDMETHODCALLTYPE Release(void) override { return 1; }

		std::set<std::string> IncludeFiles;
	};

	class ReflectionBlob : public IDxcBlob
	{
	public:
		ReflectionBlob(void const* pShaderBytecode, SIZE_T byteLength) : bytecodeSize{ byteLength }
		{
			pBytecode = const_cast<void*>(pShaderBytecode);
		}
		virtual ~ReflectionBlob() { /*non owning blob -> empty destructor*/ }
		virtual LPVOID STDMETHODCALLTYPE GetBufferPointer(void) override { return pBytecode; }
		virtual SIZE_T STDMETHODCALLTYPE GetBufferSize(void) override { return bytecodeSize; }
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppv) override
		{
			if (ppv == NULL) return E_POINTER;
			if (riid == __uuidof(IDxcBlob)) //uuid(guid_str)
				*ppv = static_cast<IDxcBlob*>(this);
			else if (riid == IID_IUnknown)
				*ppv = static_cast<IUnknown*>(this);
			else
			{ // unsupported interface
				*ppv = NULL;
				return E_NOINTERFACE;
			}

			reinterpret_cast<IUnknown*>(*ppv)->AddRef();
			return S_OK;

		}
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override { return 1; }
		virtual ULONG STDMETHODCALLTYPE Release(void) override { return 1; }
	private:
		LPVOID pBytecode = nullptr;
		SIZE_T bytecodeSize = 0;
	};

	inline constexpr std::wstring GetTarget(EShaderStage stage, EShaderModel model)
	{
		std::wstring target = L"";
		switch (stage)
		{
		case EShaderStage::VS:
			target += L"vs_";
			break;
		case EShaderStage::PS:
			target += L"ps_";
			break;
		case EShaderStage::CS:
			target += L"cs_";
			break;
		case EShaderStage::GS:
			target += L"gs_";
			break;
		case EShaderStage::HS:
			target += L"hs_";
			break;
		case EShaderStage::DS:
			target += L"ds_";
			break;
		case EShaderStage::LIB:
			target += L"lib_";
			break;
		case EShaderStage::MS:
			target += L"ms_";
			break;
		case EShaderStage::AS:
			target += L"as_";
			break;
		default:	
			CORE_ASSERT(false, "Invalid Shader Stage");
		}
		switch (model)
		{
		case SM_5_1:
			target += L"5_1";
			break;
		case SM_6_0:
			target += L"6_0";
			break;
		case SM_6_1:
			target += L"6_1";
			break;
		case SM_6_2:
			target += L"6_2";
			break;
		case SM_6_3:
			target += L"6_3";
			break;
		case SM_6_4:
			target += L"6_4";
			break;
		case SM_6_5:
			target += L"6_5";
			break;
		case SM_6_6:
			target += L"6_6";
			break;
		default:
			break;
		}
		return target;
	}

	void FDX12ShaderCompiler::Init()
	{
		ThrowIfFailed(   DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(DxcLibrary.GetAddressOf())));
		ThrowIfFailed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(DxcCompiler.GetAddressOf())));
		ThrowIfFailed(DxcLibrary->CreateIncludeHandler(DxcIncludeHandler.GetAddressOf()));
		ThrowIfFailed(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(DxcUtils.GetAddressOf())));
	}

	bool FDX12ShaderCompiler::CompileShader(EShaderStage ShaderStage, FShaderDesc const& Desc, FShaderCompileOutput& Output, std::set<std::string>& IncludeFiles)
	{
		uint32_t CodePage = CP_UTF8;
		ComPtr<IDxcBlobEncoding> SourceBlob;
		std::string FullPath = ZConfig::GetAssestsFullPath(Desc.FileName).string();
		std::wstring ShaderSource = Utils::String2WString(FullPath);
		ThrowIfFailed(DxcLibrary->CreateBlobFromFile(ShaderSource.data(), &CodePage, &SourceBlob));
		std::wstring Name = Utils::String2WString(FileUtil::GetFilenameWithoutExtension(Desc.FileName));
		std::wstring Dir = Utils::String2WString(ZConfig::ShaderDir.string());
		std::wstring Path = Utils::String2WString(FileUtil::GetParentPath(FullPath));
		
		std::wstring Target = GetTarget(ShaderStage, Desc.Model);
		std::wstring EntryPoint = Utils::String2WString(Desc.EntryPoint[uint32_t(ShaderStage)]);

		std::vector<wchar_t const*> CompileArgs{};
		CompileArgs.push_back(Name.c_str());
		if (HasAnyFlag(Desc.Flags, EShaderCompilerFlagBit::ShaderCompilerFlag_Debug))
		{
			CompileArgs.push_back(DXC_ARG_DEBUG);
			CompileArgs.push_back(L"-Qembed_debug");
		}
		else
		{
			CompileArgs.push_back(L"-Qstrip_debug");
			CompileArgs.push_back(L"-Qstrip_reflect");
		}

		if (HasAnyFlag(Desc.Flags , EShaderCompilerFlagBit::ShaderCompilerFlag_DisableOptimization))
		{
			CompileArgs.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);
		}
		else
		{
			CompileArgs.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
		}
		CompileArgs.push_back(L"-HV 2021");

		CompileArgs.push_back(L"-E"); // Entry point name
		CompileArgs.push_back(EntryPoint.c_str());

		CompileArgs.push_back(L"-T"); // Target
		CompileArgs.push_back(Target.c_str());

		CompileArgs.push_back(L"-I"); // Add directory to include search path
		CompileArgs.push_back(Dir.c_str());

		CompileArgs.push_back(L"-I"); // Add directory to include search path
		CompileArgs.push_back(Path.c_str());

		FCustomIncludeHandler CustomIncludeHandler{};
		DxcBuffer SourceBuffer;
		SourceBuffer.Ptr = SourceBlob->GetBufferPointer();
		SourceBuffer.Size = SourceBlob->GetBufferSize();
		SourceBuffer.Encoding = 0;
		HRESULT hr;
		ComPtr<IDxcResult> DxcResult = nullptr;
		hr = DxcCompiler->Compile(
			&SourceBuffer,
			CompileArgs.data(), (uint32_t)CompileArgs.size(),
			&CustomIncludeHandler,
			IID_PPV_ARGS(DxcResult.GetAddressOf()));

		ComPtr<IDxcBlobUtf8> Errors;
		if (SUCCEEDED(DxcResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(Errors.GetAddressOf()), nullptr)))
		{
			if (Errors && Errors->GetStringLength() > 0)
			{
				if (SUCCEEDED(hr))
				{
					CORE_LOG_WARN((char*)Errors->GetBufferPointer());
				}
				else
				{
					CORE_LOG_ERROR((char*)Errors->GetBufferPointer());
					return false;
				}
			}
		}

		ComPtr<IDxcBlob> _Blob;
		ThrowIfFailed(DxcResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(_Blob.GetAddressOf()), nullptr));
		Output.Shader->SetBytecode(ShaderStage,_Blob->GetBufferPointer(), _Blob->GetBufferSize());
		IncludeFiles.merge(CustomIncludeHandler.IncludeFiles);

		ComPtr<IDxcBlob> ReflectionBlob{};
		ThrowIfFailed(DxcResult->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&ReflectionBlob), nullptr));

		const DxcBuffer ReflectionBuffer
		{
			.Ptr = ReflectionBlob->GetBufferPointer(),
			.Size = ReflectionBlob->GetBufferSize(),
			.Encoding = 0,
		};

		DxcUtils->CreateReflection(&ReflectionBuffer, IID_PPV_ARGS(&Output.ShaderReflection));

		return true;
	}

	ComPtr<ID3DBlob> FDX12ShaderCompiler::CompileShader(const std::wstring& Filename, const D3D_SHADER_MACRO* Defines, const std::string& Entrypoint, const std::string& Target)
	{
		UINT CompileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		CompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		HRESULT hr = S_OK;

		ComPtr<ID3DBlob> ByteCode = nullptr;
		ComPtr<ID3DBlob> Errors;

		hr = D3DCompileFromFile(
			Filename.c_str(),
			Defines,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			Entrypoint.c_str(),
			Target.c_str(),
			CompileFlags,
			0,
			&ByteCode,
			&Errors
		);

		if (Errors != nullptr)
		{
			OutputDebugStringA((char*)Errors->GetBufferPointer());
			if (FAILED(hr))
			{
				CORE_LOG_ERROR((char*)Errors->GetBufferPointer());
				return nullptr;
			}
			else
			{
				CORE_LOG_WARN((char*)Errors->GetBufferPointer());
			}
		}

		return ByteCode;
	}
}
