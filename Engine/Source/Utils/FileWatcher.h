#pragma once

#include <filesystem>
#include "Delegate.h"

namespace Zero
{
	DEFINITION_MULTICAST_DELEGATE(FileModifiedEvent, void, const std::string&)

	enum class EFileStatus : uint8_t
	{
		Created,
		Modified,
		Deleted
	};
	class FFileWatcher
	{
	public:
		FFileWatcher() = default;
		~FFileWatcher()
		{
			m_FileModifiedEvent.ReleaseDelegates();
		}
		void AddPathToWatch(std::string const& Path, bool bRecursive = true)
		{
			if (bRecursive)
			{
				for (auto& SubPath : std::filesystem::recursive_directory_iterator(Path))
				{
					if (SubPath.is_regular_file())
					{
						m_FilesMap[SubPath.path().string()] = std::filesystem::last_write_time(SubPath);
					}
				}
			}
			else
			{
				for (auto& SubPath : std::filesystem::directory_iterator(Path))
				{
					if (SubPath.is_regular_file())
					{
						m_FilesMap[SubPath.path().string()] = std::filesystem::last_write_time(SubPath);
					}
				}
			}
		}

		void CheckWatchedFiles()
		{
			for (auto const& [File, FileTime] : m_FilesMap)
			{
				auto CurrentFileLastWriteTime = std::filesystem::last_write_time(File);
				if (m_FilesMap[File] != CurrentFileLastWriteTime)
				{
					m_FilesMap[File] = CurrentFileLastWriteTime;
					m_FileModifiedEvent.Broadcast(File);
				}
			}
		}

	private:
		std::vector<std::string> m_PathstoWatch;
		std::map<std::string, std::filesystem::file_time_type> m_FilesMap;
		FileModifiedEvent m_FileModifiedEvent;
	};
}