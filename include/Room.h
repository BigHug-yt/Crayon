#pragma once

#include <sstream>

namespace Crayon {

	const std::string Key = "\n";
	const std::string Value = ": ";
	const std::string BeginSeq = "\n";
	const std::string EndSeq = "";
	const std::string BeginBlock = "- ";
	const std::string EndBlock = "";

	class Room {

	public:
		Room& Write(const std::string& v);
		const char* c_str();
	private:
		std::string m_Raw;
	};

	inline Room& operator<< (Room& room, const std::string& v) { return room.Write(v); }
	inline Room& operator<< (Room& room, const char* v) { return room.Write(std::string(v)); }
}
