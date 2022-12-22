#include "Room.h"

#include "Crayon.h"

namespace Crayon {

	void Room::NewLine() {

		m_Raw.append("\n");
		for (uint32_t i = 0; i < m_ListDepth; i++)
			m_Raw.append("\t");
	}

	Room& Room::Write(const std::string& v) {

		if (!m_IsDesigning)
			m_Raw.append(", ");
		m_Raw.append(v);
		m_IsDesigning = false;
		return *this;
	}

	Room& Room::Design(const DesignType& v) {

		m_IsDesigning = true;
		switch (v) {
			case DesignType::Key: {
				if (!m_Raw.empty())
					NewLine();
				m_History.push(v);
				break;
			}
			case DesignType::Value: {
				m_Raw.append(": ");
				m_History.push(v);
				break;
			}
			case DesignType::BeginSubset: {
				DesignType top = m_History.top();
#if CRAYON_SAMELINE_BRACKETS
				if (top == DesignType::EndSubset || top == DesignType::BeginSubset)
					NewLine();
				m_Raw.append("{");
				m_ListDepth++;
#else
				NewLine();
				m_Raw.append("{");
				m_ListDepth++;
#endif
				m_History.push(v);
				break;
			}
			case DesignType::EndSubset: {
				m_ListDepth--;
				NewLine();
				m_Raw.append("}");
				m_History.push(v);
				break;
			}
			// TODO: Make list a one liner ( { x, y, z, ... } )
			case DesignType::BeginList: {
				m_Raw.append("[ ");
				m_History.push(v);
				break;
			}
			case DesignType::EndList: {
				m_Raw.append(" ]");
				m_History.push(v);
				break;
			}
		}
		return *this;
	}

	const char* Room::c_str() {

		return m_Raw.c_str();
	}
}
