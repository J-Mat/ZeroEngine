#pragma once
#include "Header.h"
#include <direct.h>
#include <iostream>

namespace Zero
{
	namespace Utils
	{
		namespace StringUtils
		{
			static inline std::vector<std::string> Compact(const std::vector<std::string>& tokens)
			{
				std::vector<std::string> compacted;

				for (size_t i = 0; i < tokens.size(); ++i)
				{
					if (!tokens[i].empty())
					{
						compacted.push_back(tokens[i]);
					}
				}

				return compacted;
			}

			static inline bool Contains(const std::string& Str, const std::string& Item)
			{
				return Str.find(Item) != std::string::npos;
			}


			static std::string& Trim(std::string& s)
			{
				if (s.empty())
				{
					return s;
				}

				s.erase(0, s.find_first_not_of(" "));
				s.erase(s.find_last_not_of(" ") + 1);
				return s;
			}

			static inline std::vector<std::string> Split(const std::string& str, const std::string& delim, const bool trim_empty = true)
			{
				size_t pos, last_pos = 0, len;
				std::vector<std::string> tokens;

				while (true)
				{
					pos = str.find(delim, last_pos);
					if (pos == std::string::npos)
					{
						pos = str.size();
					}

					
					len = pos - last_pos;
					if (!trim_empty || len != 0) {
						std::string Value = str.substr(last_pos, len);
						tokens.push_back(Trim(Value));
					}

					if (pos == str.size())
					{
						break;
					}
					else
					{
						last_pos = pos + delim.size();
					}
				}

				return tokens;
			}


			static inline std::string Join(const std::vector<std::string>& tokens, const std::string& delim, const bool trim_empty = false)
			{
				if (trim_empty)
				{
					return Join(Compact(tokens), delim, false);
				}
				else
				{
					std::stringstream ss;
					for (size_t i = 0; i < tokens.size() - 1; ++i)
					{
						ss << tokens[i] << delim;
					}
					ss << tokens[tokens.size() - 1];

					return ss.str();
				}
			}


			static inline std::string Repeat(const std::string& str, unsigned int times)
			{
				std::stringstream ss;
				for (unsigned int i = 0; i < times; ++i)
				{
					ss << str;
				}
				return ss.str();
			}

			static inline std::string ReplaceAll(const std::string& source, const std::string& target, const std::string& replacement)
			{
				return Join(Split(source, target, false), replacement, false);
			}

			static inline std::string ToUpper(const std::string& str)
			{
				std::string s(str);
				std::transform(s.begin(), s.end(), s.begin(), toupper);
				return s;
			}

			static inline std::string ToLower(const std::string& str)
			{
				std::string s(str);
				std::transform(s.begin(), s.end(), s.begin(), tolower);
				return s;
			}


			static inline std::string ReadFile(const std::string& FilePath)
			{
				std::ifstream ifs(FilePath.c_str());
				std::string content((std::istreambuf_iterator<char>(ifs)),
					(std::istreambuf_iterator<char>()));
				ifs.close();
				return content;
			}

			static inline void WriteFile(const std::string& filepath, const std::string& content)
			{
				std::ofstream ofs(filepath.c_str());
				ofs << content;
				ofs.close();
				return;
			}
		}

		namespace StringUtils
		{

			class ArgBase
			{
			public:
				ArgBase() {}
				virtual ~ArgBase() {}
				virtual void Format(std::ostringstream& ss, const std::string& fmt) = 0;
			};

			template <class T>
			class Arg : public ArgBase
			{
			public:
				Arg(T arg) : m_arg(arg) {}
				virtual ~Arg() {}
				virtual void Format(std::ostringstream& ss, const std::string& fmt)
				{
					ss << m_arg;
				}
			private:
				T m_arg;
			};

			class ArgArray : public std::vector < ArgBase* >
			{
			public:
				ArgArray() {}
				~ArgArray()
				{
					std::for_each(begin(), end(), [](ArgBase* p) { delete p; });
				}
			};

			static void FormatItem(std::ostringstream& ss, const std::string& item, const ArgArray& args)
			{
				int index = 0;
				int alignment = 0;
				std::string fmt;

				char* endptr = nullptr;
				index = strtol(&item[0], &endptr, 10);
				if (index < 0 || index >= args.size())
				{
					return;
				}

				if (*endptr == ',')
				{
					alignment = strtol(endptr + 1, &endptr, 10);
					if (alignment > 0)
					{
						ss << std::right << std::setw(alignment);
					}
					else if (alignment < 0)
					{
						ss << std::left << std::setw(-alignment);
					}
				}

				if (*endptr == ':')
				{
					fmt = endptr + 1;
				}

				args[index]->Format(ss, fmt);

				return;
			}

			template <class T>
			static void Transfer(ArgArray& argArray, T t)
			{
				argArray.push_back(new Arg<T>(t));
			}

			template <class T, typename... Args>
			static void Transfer(ArgArray& argArray, T t, Args&&... args)
			{
				Transfer(argArray, t);
				Transfer(argArray, args...);
			}

			template <typename... Args>
			std::string Format(const std::string& format, Args&&... args)
			{
				if (sizeof...(args) == 0)
				{
					return format;
				}

				ArgArray argArray;
				Transfer(argArray, args...);
				size_t start = 0;
				size_t pos = 0;
				std::ostringstream ss;
				while (true)
				{
					pos = format.find('{', start);
					if (pos == std::string::npos)
					{
						ss << format.substr(start);
						break;
					}

					ss << format.substr(start, pos - start);
					if (format[pos + 1] == '{')
					{
						ss << '{';
						start = pos + 2;
						continue;
					}

					start = pos + 1;
					pos = format.find('}', start);
					if (pos == std::string::npos)
					{
						ss << format.substr(start - 1);
						break;
					}

					FormatItem(ss, format.substr(start, pos - start), argArray);
					start = pos + 1;
				}

				return ss.str();
			}
		}
	}
}