#include "Node.h"

#include "Crayon.h"

#include <iostream>
#include <string>

namespace Crayon {

	Node::Node() {
		// Defective Node constructor
		m_Type = NodeType::None;
		m_IsReal = false;
		m_Raw = "";
	}

	Node::Node(const std::string& str) {

		m_Raw = str;
		m_IsReal = true;

		// determine NodeType
		m_Type = NodeType::None;
		if (m_Raw._Starts_with("[")) { m_Type = NodeType::List; }
		else if (m_Raw.find(":") != std::string::npos || m_Raw.find("{") != std::string::npos) { m_Type = NodeType::Node; }
		else {
			m_Type = NodeType::Scalar;
			try { std::stof(m_Raw); }
			catch (std::invalid_argument) { m_Type = NodeType::Tag; }
		}
		if (m_Type == NodeType::Node)
			IdentifyKeys();
		if (m_Type == NodeType::List) {
			// TODO: Split a list up into its items (store them as Nodes in m_Keys)
			IdentifyItems();
		}
	}

	Node Node::LocateAndGen(const std::string& key) {

		// Only nodes can have keys
		if (m_Type != NodeType::Node)
			return Node();

		size_t keyPos = m_Raw.find(key);
		if (keyPos != std::string::npos) {

			uint32_t crudeStart = keyPos + key.length();
			uint32_t endPos = FindEndOfValue(crudeStart);

			std::string value = StripAndClip(crudeStart, endPos);

			return Node(value);
		}
		// Node doesn't contain key
		return Node();
	}

	int Node::FindFirstKeyEnd(uint32_t start) {

		std::string sub = m_Raw.substr(start, m_Raw.size());
		size_t location = sub.find(':');
		if (location == std::string::npos)
			return -1;
		return location + start;
	}

	int Node::FindFirstItem(uint32_t start) {


		std::string sub = m_Raw.substr(start, m_Raw.size());
		size_t location = sub.find('{');
		if (location == std::string::npos)
			return -1;
		return location + start;
	}

	uint32_t Node::FindEndOfValue(uint32_t start) {

		uint32_t end = start;
		char currentChar = m_Raw[end];

		bool reachedStart = (currentChar != ' ') && (currentChar != '\n') && (currentChar != '\t') && (currentChar != ':');
		bool reachedBracket = (currentChar == '{');
		uint32_t bracketsFound = 0;
		if (reachedBracket)
			bracketsFound++;

		while (true) {

			end++;
			if (end >= m_Raw.size())
				return m_Raw.size() - 1;
			currentChar = m_Raw[end];

			if (reachedStart) {
				if ((currentChar == '\n') && (!reachedBracket))
					return end;
				if (currentChar == '{')
					bracketsFound++;
				if (currentChar == '}') {
					bracketsFound--;
					if (bracketsFound == 0)
						return end - 1;
				}
			}
			else {
				if ((currentChar != ' ') && (currentChar != '\n') && (currentChar != '\t')) {

					reachedStart = true;
					if (currentChar == '{') {

						reachedBracket = true;
						bracketsFound++;
					}
				}
			}
		}
	}

	std::string Node::StripAndClip(uint32_t start, uint32_t end) {

		uint32_t actualStart = start;
		char currentChar = m_Raw[actualStart];

		bool foundBracket = currentChar == '{';

		bool reachedActualStart = currentChar != ' ' && currentChar != '\n' && currentChar != '\t' && currentChar != ':' && currentChar != '{';
		while (!reachedActualStart) {

			actualStart++;
			currentChar = m_Raw[actualStart];

			if (currentChar == ' ' || currentChar == '\t' || currentChar == '\n' || currentChar == ':') {
				continue;
			}

			if (currentChar == '{' && !foundBracket) {
				foundBracket = true;
				continue;
			}
			reachedActualStart = true;
		}
		// found actualStart, looking for actualEnd
		uint32_t actualEnd = end;
		currentChar = m_Raw[actualEnd];

		foundBracket = currentChar == '}';

		bool reachedActualEnd = currentChar != ' ' && currentChar != '\n' && currentChar != '\t' && currentChar != ':' && currentChar != '}';
		
		while (!reachedActualEnd) {

			actualEnd--;
			currentChar = m_Raw[actualEnd];

			if (currentChar == ' ' || currentChar == '\t' || currentChar == '\n' || currentChar == ':') {
				reachedActualEnd = true;
				continue;
			}

			if (currentChar == '}') {

				if (foundBracket) {
					reachedActualEnd = true;
				}
				foundBracket = true;
				continue;
			}
			reachedActualEnd = true;
		}

		return m_Raw.substr(actualStart, actualEnd-actualStart+1);
	}

	void Node::IdentifyKeys() {

		uint32_t rawStart = 0;
		int firstKey = FindFirstKeyEnd(rawStart);	// Key: value
		int firstItem = FindFirstItem(rawStart);	// { value }
		int minimum = (firstKey != -1) ? ((firstItem != -1) ? std::min(firstKey, firstItem) : firstKey) : (firstItem != -1) ? firstItem : -1;
		if (minimum == -1) { m_IsReal = false; return; } // if Node doesn't contain keys or items, it's probably empty
		uint32_t endPos = FindEndOfValue(minimum);
		std::string value = StripAndClip(minimum, endPos);
		m_Keys.push_back(Node(value));
		rawStart = endPos;
		while (true) {
			firstKey = FindFirstKeyEnd(rawStart);	// Key: value
			firstItem = FindFirstItem(rawStart);	// { value }
			minimum = (firstKey != -1) ? ((firstItem != -1) ? std::min(firstKey, firstItem) : firstKey) : (firstItem != -1) ? firstItem : -1;
			if (minimum == -1) { break; } // no more items or keys found
			endPos = FindEndOfValue(minimum);
			value = StripAndClip(minimum, endPos);
			m_Keys.push_back(Node(value));
			rawStart = endPos;
		}
	}

	void Node::IdentifyItems() {

		uint32_t pointer = 0;
		uint32_t itemStart = 0;
		uint32_t itemEnd = 0;
		bool hasStarted = false;
		char currentChar = m_Raw[pointer];
		while (currentChar != ']') {
			if (currentChar != ' ' && currentChar != '[') {

				if (!hasStarted)
					itemStart = pointer;
				hasStarted = true;
			}
			if (currentChar == ',') {
				hasStarted = false;
				itemEnd = pointer - 1;
				m_Keys.push_back(Node(m_Raw.substr(itemStart, itemEnd - itemStart)));
			}

			pointer++;
			currentChar = m_Raw[pointer];
		}
		itemEnd = pointer - 1;
		m_Keys.push_back(Node(m_Raw.substr(itemStart, itemEnd - itemStart)));
	}

	template <>
	std::string Node::as<std::string>() const {
		// return t if conversion goes wrong
		if (!m_IsReal)
			throw std::exception();

		switch (m_Type) {
			case NodeType::Scalar: { return m_Raw; }
			case NodeType::Tag: { return m_Raw; }
			case NodeType::List: { return m_Raw; }
			case NodeType::Node: { throw std::exception(); }
			case NodeType::None: { throw std::exception(); }
		}
	}
	template <>
	uint16_t Node::as<uint16_t>() const {
		// return t if conversion goes wrong
		if (!m_IsReal)
			throw std::exception();

		switch (m_Type) {
			case NodeType::Scalar: { return (uint16_t)stof(m_Raw); }
			case NodeType::Tag: { throw std::exception(); }
			case NodeType::List: { throw std::exception(); }
			case NodeType::Node: { throw std::exception(); }
			case NodeType::None: { throw std::exception(); }
		}
	}
	template <>
	uint32_t Node::as<uint32_t>() const {
		// return t if conversion goes wrong
		if (!m_IsReal)
			throw std::exception();

		switch (m_Type) {
			case NodeType::Scalar: { return (uint32_t)stof(m_Raw); }
			case NodeType::Tag: { throw std::exception(); }
			case NodeType::List: { throw std::exception(); }
			case NodeType::Node: { throw std::exception(); }
			case NodeType::None: { throw std::exception(); }
		}
	}
	template <>
	uint64_t Node::as<uint64_t>() const {
		// return t if conversion goes wrong
		if (!m_IsReal)
			throw std::exception();

		switch (m_Type) {
			case NodeType::Scalar: { return (uint64_t)stof(m_Raw); }
			case NodeType::Tag: { throw std::exception(); }
			case NodeType::List: { throw std::exception(); }
			case NodeType::Node: { throw std::exception(); }
			case NodeType::None: { throw std::exception(); }
		}
	}
	template <>
	int Node::as<int>() const {
		// return t if conversion goes wrong
		if (!m_IsReal)
			throw std::exception();

		switch (m_Type) {
			case NodeType::Scalar: { return (int)stof(m_Raw); }
			case NodeType::Tag: { throw std::exception(); }
			case NodeType::List: { throw std::exception(); }
			case NodeType::Node: { throw std::exception(); }
			case NodeType::None: { throw std::exception(); }
		}
	}
	template <>
	float Node::as<float>() const {
		// return t if conversion goes wrong
		if (!m_IsReal)
			throw std::exception();

		switch (m_Type) {
			case NodeType::Scalar: { return (float)stof(m_Raw); }
			case NodeType::Tag: { throw std::exception(); }
			case NodeType::List: { throw std::exception(); }
			case NodeType::Node: { throw std::exception(); }
			case NodeType::None: { throw std::exception(); }
		}
	}
	template <>
	double Node::as<double>() const {
		// return t if conversion goes wrong
		if (!m_IsReal)
			throw std::exception();

		switch (m_Type) {
			case NodeType::Scalar: { return (double)stof(m_Raw); }
			case NodeType::Tag: { throw std::exception(); }
			case NodeType::List: { throw std::exception(); }
			case NodeType::Node: { throw std::exception(); }
			case NodeType::None: { throw std::exception(); }
		}
	}
	template <>
	long double Node::as<long double>() const {
		// return t if conversion goes wrong
		if (!m_IsReal)
			throw std::exception();

		switch (m_Type) {
			case NodeType::Scalar: { return (long double)stof(m_Raw); }
			case NodeType::Tag: { throw std::exception(); }
			case NodeType::List: { throw std::exception(); }
			case NodeType::Node: { throw std::exception(); }
			case NodeType::None: { throw std::exception(); }
		}
	}
	template <>
	bool Node::as<bool>() const {
		// return t if conversion goes wrong
		if (!m_IsReal)
			throw std::exception();

		switch (m_Type) {
			case NodeType::Scalar: { return m_Raw != "0"; }
			case NodeType::Tag: {
				if (m_Raw == "true" || m_Raw == "True" || m_Raw == "TRUE" || m_Raw == "yes" || m_Raw == "Yes" || m_Raw == "YES")
					return true;
				else if (m_Raw == "false" || m_Raw == "False" || m_Raw == "FALSE" || m_Raw == "no" || m_Raw == "No" || m_Raw == "NO")
					return false;
				throw std::exception();
			}
			case NodeType::List: { throw std::exception(); }
			case NodeType::Node: { throw std::exception(); }
			case NodeType::None: { throw std::exception(); }
		}
	}
}
