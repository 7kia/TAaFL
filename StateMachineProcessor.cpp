#include "stdafx.h"
#include "StateMachineProcessor.h"

using namespace json_spirit;
using namespace std;

bool operator ==(vector<SCell> const &firstCells, vector<SCell> const &secondCells)
{
	if (firstCells.size() != secondCells.size())
	{
		return false;
	}

	for (size_t index = 0; index != firstCells.size(); ++index)
	{
		if (firstCells[index].state != secondCells[index].state)
		{
			return false;
		}
	}

	return true;
}

vector<SCell> GetColumn(StateTable const &sm, size_t columnNumber)
{
	vector<SCell> cells;
	// Пропускаем первую колонку
	for (auto it = sm.begin() + 1; it != sm.end(); ++it)
	{
		cells.push_back((*it)[columnNumber]);
	}
	return cells;
}

CStateMachineProcessor::CStateMachineProcessor(std::string const& input, std::string const& output)
	: m_isFirstTimeWrite(true)
	, m_output(output)
{
	ifstream inputFile(input);
	Value value;
	read(inputFile, value);
	auto stateMachinesArray = value.get_obj().at(0).value_.get_array();
	for (size_t index = 0; index < stateMachinesArray.size(); index++)
	{
		m_stateMachines.push_back(CStateMachine(stateMachinesArray.at(index).get_obj()));
	}
}

void CStateMachineProcessor::WriteToFile()
{
	ofstream out(m_output);
	Object mainObj;
	mainObj.push_back(Pair("state machine", m_jsonStateMachines));
	write(mainObj, out, json_spirit::pretty_print);
}

json_spirit::Object CStateMachineProcessor::ToJson(CStateMachine const& sm)
{
	Object stateMachine;
	stateMachine.push_back(Pair("id", sm.GetId()));
	stateMachine.push_back(Pair("type", sm.GetType()));
	stateMachine.push_back(Pair("states", GetStates(sm)));
	stateMachine.push_back(Pair("transitions", GetTransitions(sm)));
	return stateMachine;
}

json_spirit::Array CStateMachineProcessor::GetStates(CStateMachine const& sm)
{
	Array states;
	auto smStates = sm.GetTable()[0];
	for (size_t index = 1; index < smStates.size(); index++)
	{
		Object state = {Pair("id", smStates[index].state)};
		if (smStates[index].state != "")
		{
			state.push_back(Pair("output", smStates[index].outputSymbol));
		}
		states.push_back(state);
	}
	return states;
}

json_spirit::Array CStateMachineProcessor::GetTransitions(CStateMachine const& sm)
{
	Array transitions;
	auto states = sm.GetTable()[0];
	for (size_t row = 1; row < sm.GetTable().size(); row++)
	{
		for (size_t col = 1; col < states.size(); col++)
		{
			if (sm.GetTable()[row][col].state != "")
			{
				Object cell;
				cell.push_back(Pair("input", sm.GetTable()[row][0].state));
				if (states[col].outputSymbol == "")
				{
					cell.push_back(Pair("output", sm.GetTable()[row][col].outputSymbol));
				}
				cell.push_back(Pair("from", states[col].state));
				cell.push_back(Pair("to", sm.GetTable()[row][col].state));
				transitions.push_back(cell);
			}
		}
	}
	return transitions;
}

CStateMachine & CStateMachineProcessor::Get(string const& id)
{
	auto it = find_if(m_stateMachines.begin()
					, m_stateMachines.end()
					, [id](CStateMachine const& sm) 
					{
						return sm.GetId() == id; 
					} );

	if (it == m_stateMachines.end())
	{
		throw invalid_argument(NOT_EXIST_THIS_STATE_MACHINE);
	}

	return *it;
}


void CStateMachineProcessor::TransferToMeale(CStateMachine & sm)
{
	if (sm.GetType() != MEELE_NAME)
	{
		map<string, string> mooreStates;
		for (auto const & cell : sm.GetTable()[0])
		{
			mooreStates.insert({ cell.state, cell.outputSymbol });
		}
		for (auto & row : sm.GetTable())
		{
			for (auto & cell : row)
			{
				auto it = mooreStates.find(cell.state);
				if (it != mooreStates.end())
				{
					cell.outputSymbol = it->second;
				}
			}
		}
		for (auto & cell : sm.GetTable()[0])
		{
			cell.outputSymbol = "";
		}
		sm.SetType(MEELE_NAME);
		m_jsonStateMachines.push_back(ToJson(sm));
	}

}

States CStateMachineProcessor::GetNewStates(StateTable const &meale)
{
	States mooreStates;
	int count = 0;
	for (auto & row : meale)
	{
		for (auto & cell : row)
		{
			if (!cell.outputSymbol.empty()
				&& (find_if(mooreStates.begin()
							, mooreStates.end()
							, [&cell](const auto  &state)
							{ 
								return state.second == cell; 
							}
					) == mooreStates.end()) )
			{
				mooreStates.emplace(NAME_NEW_STATE + to_string(count), cell);
				count++;
			}
		}
	}

	return mooreStates;
}

void CStateMachineProcessor::TransferToMoore(CStateMachine & sm)
{
	if (sm.GetType() == MOORE_NAME)
	{
		throw std::logic_error(IMPOSSIBLE_CONVERT_TO_THIS_TYPE);
	}

	StateTable & smTable = sm.GetTable();
	auto mooreStates = GetNewStates(smTable);
	StateTable transferedTable(1);
	// |this||    |...
	// |    ||    |...
	// |    ||    |...
	transferedTable[0].push_back({ "", "" });

	////////////////////////////////////
	// Первая колонка с состояниями
	for (auto &state : mooreStates)
	{
		transferedTable[0].push_back(SCell(state.first, state.second.outputSymbol));
	}
	////////////////////////////////////

	////////////////////////////////////
	// Выделение места
	for (size_t index = 1; index < smTable.size(); ++index)
	{
		vector<SCell> cells = vector<SCell>(transferedTable[0].size());
		transferedTable.push_back(cells);
	}
	////////////////////////////////////

	////////////////////////////////////
	// Добавление входных символов
	int indexRow = 1;
	for (auto &input : smTable)
	{
		if (input[0] != SCell())
		{
			transferedTable[indexRow][0] = input[0];
			++indexRow;
		}
	}
	////////////////////////////////////


	for (size_t index = 1; index < transferedTable[0].size(); ++index)
	{
		auto mooreState = mooreStates.at(transferedTable[0][index].state).state;
		// Ищем состояние сопадающее с состоянием
		for (size_t mooreIndex = 1; mooreIndex < (smTable[0].size() - 1); mooreIndex++)
		{
			if (smTable[0][mooreIndex].state == mooreState)
			{
				for (size_t thirdIndex = 1; thirdIndex < smTable.size(); ++thirdIndex)
				{
					SCell state2 = smTable[mooreIndex][thirdIndex];
					auto it = find_if(mooreStates.begin()
										, mooreStates.end()
										, [&state2](auto const &state) 
										{
											return state.second == state2; 
										}
					
									);

					if (it != mooreStates.end())
					{
						transferedTable[mooreIndex][thirdIndex].state = it->first;
					}
				}
			}
		}


	}

	sm.GetTable() = transferedTable;
	sm.SetType(MOORE_NAME);

	m_jsonStateMachines.push_back(ToJson(sm));
}

void CStateMachineProcessor::Determine(CStateMachine & sm)
{
	StateTable dTable;
	dTable.push_back({ {"", ""}, {sm.GetTable()[0][1]} });
	for (size_t index = 1; index < sm.GetTable().size(); index++)
	{
		dTable.push_back({ { sm.GetTable()[index][0] } });
	}
	auto startSize = dTable[0].size();
	size_t addedStatesCount = 0;
	for (size_t col = 1; col < startSize + addedStatesCount; col++)
	{
		for (size_t row = 1; row < dTable.size(); row++)
		{
			//split state -from- and add to cell -to- each of result
			//if -to- state is new then add it to dTable states and addedStatesCount++
			vector<string> splittedState;
			boost::split(splittedState, dTable[0][col].state, bind2nd(equal_to<char>(), ';'));
			SCell cell;
			for (auto & state : splittedState)
			{
				auto it = find_if(sm.GetTable()[0].begin(), sm.GetTable()[0].end(),
					[&](SCell const& pair) {return pair.state == state; });
				auto pos = it - sm.GetTable()[0].begin();

				if ((sm.GetTable()[row][pos].state != "")
					&& 
					(cell.state.find(sm.GetTable()[row][pos].state) == string::npos))
				{
					cell.state += sm.GetTable()[row][pos].state;
					cell.state += ";";
				}
			}

			dTable[row].push_back(cell);
			std::string & cellFirst = dTable[row][col].state;
			cellFirst.erase(cellFirst.end() - 1, cellFirst.end());

			vector<string> compareVec;
			boost::split(compareVec, cellFirst, bind2nd(equal_to<char>(), ';'));
			std::sort(compareVec.begin(), compareVec.end());

			auto it = find_if(dTable[0].begin()
							, dTable[0].end()
							, [&](SCell const& pair) 
								{
									vector<string> compare2;
									boost::split(compare2, pair.state, bind2nd(equal_to<char>(), ';'));
									std::sort(compare2.begin(), compare2.end());
									return compare2 == compareVec;
								}
							);
			if (it == dTable[0].end())
			{
				dTable[0].push_back({ cellFirst, "" });
				addedStatesCount++;
			}
		}
	}

	map<string, string> newStates;
	auto stateName = NAME_NEW_STATE;
	for (size_t index = 1; index < dTable[0].size(); index++)
	{
		vector<string> state;
		boost::split(state, dTable[0][index].state, bind2nd(equal_to<char>(), ';'));
		sort(state.begin(), state.end());
		string strState;
		strState = accumulate(state.begin(), state.end(), strState);
		newStates.emplace(strState, stateName + to_string(index));
	}

	for (size_t row = 0; row < dTable.size(); row++)
	{
		for (size_t col = 1; col < dTable[0].size(); col++)
		{

			vector<string> state;
			boost::split(state, dTable[row][col].state, bind2nd(equal_to<char>(), ';'));
			sort(state.begin(), state.end());

			string strState;
			strState = accumulate(state.begin(), state.end(), strState);
			dTable[row][col].state = newStates.at(strState);
		}
	}
	sm.GetTable() = dTable;
	m_jsonStateMachines.push_back(ToJson(sm));
}

StateTable CStateMachineProcessor::GetTableEquivalenceClass(StateTable resourceST, StateTable const & originalST)
{
	int classNumber = 1;
	map<string, int> classes;
	while (resourceST[0].size() != 1)
	{
		auto currentColumn = GetColumn(resourceST, 1);
		classes.emplace(resourceST[0][1].state, classNumber);

		for (size_t index = 0; index != resourceST.size(); ++index)
		{
			auto &row = resourceST[index];
			row.erase(row.begin() + 1);
		}

		////////////////////////////////////////////////
		// добавление к классу эквивалентности
		for (size_t index = 1; index < resourceST[0].size(); ++index)
		{
			auto cmpColumn = GetColumn(resourceST, index);
			if (cmpColumn == currentColumn)
			{
				classes.emplace(resourceST[0][index].state, classNumber);
			}
		}
		++classNumber;
		////////////////////////////////////////////////
	}

	auto outputST = originalST;
	for (auto index = 1; index != outputST.size(); ++index)
	{
		for (auto secondIndex = 1; secondIndex != outputST[0].size(); ++secondIndex)
		{
			auto state = classes.find(originalST[index][secondIndex].state);
			if (state != classes.end())
			{
				outputST[index][secondIndex].state = to_string(state->second);
			}
		}
	}

	return outputST;
}

void CStateMachineProcessor::Minimize(CStateMachine & sm)
{
	if (sm.GetType() != MEELE_NAME)
	{
		TransferToMeale(sm);
	}

	StateTable tempCopyST = sm.GetTable();

	StateTable oldSTCpy = GetTableEquivalenceClass(tempCopyST, sm.GetTable());
	StateTable currentSTCpy;
	do
	{
		currentSTCpy = GetTableEquivalenceClass(oldSTCpy, sm.GetTable());
		oldSTCpy = currentSTCpy;

	} 
	while (oldSTCpy != currentSTCpy);

	sm.GetTable() = oldSTCpy;
	m_jsonStateMachines.push_back(ToJson(sm));
}

CStateMachineProcessor::~CStateMachineProcessor()
{
}
