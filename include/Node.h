#pragma once

#include <sstream>
#include <vector>

namespace Crayon {

	enum class NodeType {
		Scalar,
		Tag,
		List,
		Node,
		None // Deserializing went wrong, Node unusable 
	};

	class Node {

	public:
		Node();
		Node(const std::string& str);

		Node operator[](const std::string& index) { return LocateAndGen(index); }
		Node operator[](const char* index) { return LocateAndGen(std::string(index)); }

		operator bool() const { return m_IsReal; }

		inline std::size_t size() const { return m_Keys.size(); }
		inline auto begin() { return m_Keys.begin(); }
		inline auto end() { return m_Keys.end(); }
		inline auto begin() const { return m_Keys.begin(); }
		inline auto end() const { return m_Keys.end(); }

		template <typename T>
		inline T as() const;
	private:
		Node LocateAndGen(const std::string& key);	// finds Node corresponding to key
		int FindFirstKeyEnd(uint32_t start);	// basically finds first ':', returns -1 if none found
		int FindFirstItem(uint32_t start);		// basically finds first '{', returns -1 if none found
		uint32_t FindEndOfValue(uint32_t start);
		std::string StripAndClip(uint32_t start, uint32_t end);	// returns polished substring of m_Raw to create a new Node
		void IdentifyKeys();
		void IdentifyItems();
	private:
		NodeType m_Type;
		bool m_IsReal = false;
		std::string m_Raw;
		std::vector<Node> m_Keys;
	};
}
