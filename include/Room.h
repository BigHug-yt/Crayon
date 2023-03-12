#pragma once

#include <sstream>
#include <stack>

namespace Crayon {

	enum DesignType {

		Key,
		Value,
		BeginList,
		EndList,
		BeginSubset,
		EndSubset
	};

	class Room {

	public:
		Room& Write(const std::string& v);
		Room& Design(const DesignType& v);
		void NewLine();
		const char* c_str();
	private:
		std::string m_Raw;

		bool m_IsDesigning = false;
		uint32_t m_ListDepth = 0;
		std::stack<DesignType> m_History;
	};

	inline Room& operator<< (Room& room, const std::string& v) { return room.Write(v); }
	inline Room& operator<< (Room& room, const char* v) { return room.Write(std::string(v)); }
	inline Room& operator<< (Room& room, bool v) { return room.Write((v) ? "true" : "false"); }
	inline Room& operator<< (Room& room, int v) { return room.Write(std::to_string(v)); }
	inline Room& operator<< (Room& room, uint16_t v) { return room.Write(std::to_string(v)); }
	inline Room& operator<< (Room& room, uint32_t v) { return room.Write(std::to_string(v)); }
	inline Room& operator<< (Room& room, uint64_t v) { return room.Write(std::to_string(v)); }
	inline Room& operator<< (Room& room, float v) { return room.Write(std::to_string(v)); }
	inline Room& operator<< (Room& room, double v) { return room.Write(std::to_string(v)); }
	inline Room& operator<< (Room& room, long double v) { return room.Write(std::to_string(v)); }

	inline Room& operator<< (Room& room, const DesignType d) { return room.Design(d); }
}
