#pragma once
#include "StateMachine.h"

typedef std::map<std::string, SCell> States;

static const std::string IMPOSSIBLE_CONVERT_TO_THIS_TYPE = "Impossible convert to this type!!!";

class CStateMachineProcessor
{
public:
	CStateMachineProcessor() = delete;
	CStateMachineProcessor(std::string const& input, std::string const& output);

	~CStateMachineProcessor();

	void								WriteToFile();
	void								TransferToMeale(CStateMachine & sm);
	void								TransferToMoore(CStateMachine & sm);
	void								Determine(CStateMachine & sm);
	void								Minimize(CStateMachine & sm);
	CStateMachine &						Get(std::string const& id);
private:
	StateTable							AllocateOfEquivalenceClass(StateTable resourceST
																	, StateTable const &originalST);
	json_spirit::Object					ToJson(CStateMachine const& sm);
	json_spirit::Array					GetStates(CStateMachine const& sm);
	json_spirit::Array					GetTransitions(CStateMachine const& sm);
	States								GetNewStates(StateTable const &meale);

private:
	std::vector<CStateMachine>			m_stateMachines;
	json_spirit::Array					m_jsonStateMachines;
	bool								m_isFirstTimeWrite;
	std::string							m_output;

};

