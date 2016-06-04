#include "StateMachine.h"
using namespace std;
using namespace json_spirit;

CStateMachine::CStateMachine(json_spirit::Object const& smData)
{
	auto type = smData.at(1).value_.get_str();
	m_id = smData.at(0).value_.get_str();
	if (type == MEELE_NAME)
	{
		m_table = GetMealeTable(smData);
		m_type = MEELE_NAME;
	}
	else if (type == MOORE_NAME)
	{
		m_table = GetMooreTable(smData);
		m_type = MOORE_NAME;
	}
}

StateTable & CStateMachine::GetTable()
{
	return m_table;
}

StateTable const& CStateMachine::GetTable() const
{
	return m_table;
}

std::string const& CStateMachine::GetType() const
{
	return m_type;
}

void CStateMachine::SetType(std::string const& type)
{
	m_type = type;
}

std::string CStateMachine::GetId() const
{
	return m_id;
}

Array CStateMachine::GetArray(Object const &stateMachine, string const& key)
{
	Array statesArray;

	for (size_t i = 0; i < stateMachine.size(); ++i)
	{
		if (stateMachine[i].name_ == key)
		{
			return stateMachine[i].value_.get_array();
		}
	}
	return{};
}

StateTable CStateMachine::GetMealeTable(Object const &sm)
{
	auto states = GetArray(sm, "states");

	StateTable table(1);
	table[0].push_back({"", ""});

	for (auto state : states)
	{
		SCell cellState;
		cellState.state = state.get_obj().at(0).value_.get_str();
		cellState.outputSymbol = "";
		table[0].push_back(cellState);

	}

	auto columnsCount = table[0].size();

	auto transitions = GetArray(sm, "transitions");
	vector<string > inputs;

	for (auto & transition : transitions)
	{
		auto input = transition.get_obj()[0].value_.get_str();
		auto output = transition.get_obj()[1].value_.get_str();
		auto from = transition.get_obj()[2].value_.get_str();
		auto to = transition.get_obj()[3].value_.get_str();
		auto iterator = std::find(inputs.begin(), inputs.end(), input);
		auto iter = find_if(table[0].begin(), table[0].end(), [&](auto const& pair)
		{
			return pair.state == from;
		});

		auto columnPos = iter - table[0].begin();

		if (iterator == inputs.end())
		{
			auto vec = vector<SCell>(columnsCount);
			vec[0] = { input, "" };
			vec[columnPos] = { to, output };
			table.push_back(vec);
			inputs.push_back(input);
		}
		else
		{
			auto rowWithInput = find_if(table.begin(), table.end(), [&](auto const& row)
			{
				return row[0].state == input;
			});
			rowWithInput->at(columnPos) = { to, output };
		}
	}

	return table;
}

StateTable CStateMachine::GetMooreTable(Object const &sm)
{
	auto table = StateTable(1);
	table[0].push_back({"", ""});

	auto states = GetArray(sm, "states");

	for (auto & state : states)
	{
		SCell cellState;
		cellState.state = state.get_obj()[0].value_.get_str();
		cellState.outputSymbol = state.get_obj()[1].value_.get_str();
		table[0].push_back(cellState);
	}

	auto columnsCount = table[0].size();

	auto transitions = GetArray(sm, "transitions");
	vector<string> inputs;

	for (auto & transition : transitions)
	{
		auto input = transition.get_obj()[0].value_.get_str();
		auto from = transition.get_obj()[1].value_.get_str();
		auto to = transition.get_obj()[2].value_.get_str();
		auto iterator = std::find(inputs.begin(), inputs.end(), input);
		auto iter = find_if(table[0].begin(), table[0].end(), [&](auto const& pair)
		{
			return pair.state == from;
		});
		auto columnPos = iter - table[0].begin();
		if (iterator == inputs.end())
		{
			auto vec = vector<SCell>(columnsCount);
			vec[0] = { input, "" };
			vec[columnPos] = { to, "" };
			table.push_back(vec);
			inputs.push_back(input);
		}
		else
		{
			auto rowWithInput = find_if(table.begin(), table.end(), [&](auto const& row)
			{
				return row[0].state == input;
			});
			rowWithInput->at(columnPos) = { to, "" };
		}
	}
	return table;
}

SCell::SCell()
{
}

SCell::SCell(std::string state, std::string outputSymbol)
	: state(state)
	, outputSymbol(outputSymbol)
{
}

bool operator==(const SCell & first, const SCell & second)
{
	return (first.state == second.state)
			&&
			(first.outputSymbol == second.outputSymbol);
}

bool operator!=(const SCell & first, const SCell & second)
{
	return !(first == second);
}
