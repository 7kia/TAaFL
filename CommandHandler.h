#pragma once
#include "StateMachineProcessor.h"

typedef std::function<void(std::istream & args)> Action;

class CCommandHandler
{
public:
	CCommandHandler() = delete;
	CCommandHandler(CStateMachineProcessor & sm, std::istream & input);

	~CCommandHandler() = default;

	void								HandleCommand();
private:
	void								ConvertToMeale(std::istream & strm);
	void								ConvertToMoore(std::istream & strm);
	void								Minimize(std::istream & strm);
	void								WriteOutputToFile(std::istream & strm);
	std::istream &						m_input;
	std::map<std::string, Action>		m_actions;
	CStateMachineProcessor &			m_smProcessor;
};

