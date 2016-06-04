#pragma once
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <functional>
#include <boost/algorithm/string.hpp>
#include <numeric>

#include "json_spirit/json_spirit.h"

struct SCell
{
	SCell();
	SCell(const std::string & state, const std::string & outputSymbol);

	std::string state;
	std::string outputSymbol;

	friend bool operator==(const SCell & first, const SCell & second);
	friend bool operator!=(const SCell & first, const SCell & second);
};

typedef std::vector<std::vector<SCell>> StateTable;

static const std::string MOORE_NAME = "moore";
static const std::string MEELE_NAME = "meale";

class CStateMachine
{
public:
	CStateMachine() = delete;
	CStateMachine(json_spirit::Object const& smData);

	StateTable &						GetTable();
	StateTable const &					GetTable() const;
	std::string const &					GetType() const;
	void								SetType(std::string const& type);
	std::string							GetId() const;
private:
	StateTable							GetMealeTable(json_spirit::Object const &object);
	StateTable							GetMooreTable(json_spirit::Object const &object);
	json_spirit::Array					GetArray(json_spirit::Object const &stateMachine, std::string const& key);

private:
	StateTable							m_table;
	std::string							m_type;
	std::string							m_id;
};