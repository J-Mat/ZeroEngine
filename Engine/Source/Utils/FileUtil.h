#pragma once

#include <filesystem>

namespace Zero
{
	namespace FileUtil
	{
		inline std::string GetParentPath(std::string const& complete_path)
		{
			std::filesystem::path p(complete_path);

			return p.parent_path().string();
		}

		inline std::string GetFilename(std::string const& complete_path)
		{
			std::filesystem::path p(complete_path);

			return p.filename().string();
		}

		inline std::string GetFilenameWithoutExtension(std::string const& complete_path)
		{
			std::filesystem::path p(complete_path);
			return p.filename().replace_extension().string();
		}

		inline bool FileExists(std::string const& file_path)
		{
			std::filesystem::path p(file_path);
			return std::filesystem::exists(p);
		}

		inline std::string GetExtension(std::string const& path)
		{
			std::filesystem::path p(path);
			return p.extension().string();
		}

		inline void NormalizePathInline(std::string& file_path)
		{
			for (char& c : file_path)
			{
				if (c == '\\')
				{
					c = '/';
				}
			}
			if (file_path.find("./") == 0)
			{
				file_path = std::string(file_path.begin() + 2, file_path.end());
			}
		}

		inline std::string NormalizePath(std::string const& file_path)
		{
			std::string output = std::string(file_path.begin(), file_path.end());
			NormalizePathInline(output);
			return output;
		}

		inline bool ResolveRelativePaths(std::string& path)
		{
			while (true)
			{
				size_t index = path.rfind("../");
				if (index == std::string::npos)
					break;
				size_t idx0 = path.rfind('/', index);
				if (idx0 == std::string::npos)
					return false;
				idx0 = path.rfind('/', idx0 - 1);
				if (idx0 != std::string::npos)
					path = path.substr(0, idx0 + 1) + path.substr(index + 3);
			}
			return true;
		}
	}
}