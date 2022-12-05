#include "include/Crayon.h"

namespace Crayon {

	void Room::operator<< (const std::string& key) {

	}

	const char* Room::c_str() {

		return m_Raw.c_str();
	}
}
