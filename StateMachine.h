#pragma once
#include <vector>
#include "json_spirit/json_spirit.h"

//typedef std::pair<std::string, std::string> SCell;

static const std::string MEELE_NAME = "meale";
static const std::string MOORE_NAME = "moore";

struct SCell
{
	SCell();
	SCell(std::string state, std::string outputSymbol); // TODO : rename

	std::string state;
	std::string outputSymbol;

	friend bool operator==(const SCell & first, const SCell & second);
	friend bool operator!=(const SCell & first, const SCell & second);

};

typedef std::vector<std::vector<SCell>> StateTable;

class CStateMachine
{
public:
	CStateMachine() = delete;
	CStateMachine(json_spirit::Object const& smData);

	StateTable &					GetTable();
	StateTable const&				GetTable() const;
	std::string const&				GetType() const;
	void							SetType(std::string const& type);
	std::string						GetId() const;
private:
	StateTable						GetMealeTable(json_spirit::Object const &object);
	StateTable						GetMooreTable(json_spirit::Object const &object);
	json_spirit::Array				GetArray(json_spirit::Object const &stateMachine
											, std::string const& key);

private:
	StateTable						m_table;
	std::string						m_type;
	std::string						m_id;
};