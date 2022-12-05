#include "Room.h"

namespace Crayon {

	Room& Room::Write(const std::string& v) {

		m_Raw.append(v);
		return *this;
	}

	const char* Room::c_str() {

		return m_Raw.c_str();
	}
}
