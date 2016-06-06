#include "stdafx.h"
#include "CommandHandler.h"
using namespace std::placeholders;
using namespace std;

CCommandHandler::CCommandHandler(CStateMachineProcessor & sm, istream & input)
	:m_input(input)
	, m_smProcessor(sm)
	, m_actions({
		{ "translate-mil-to-mur", bind(&CCommandHandler::ConvertToMoore, this, _1) },
		{ "translate-mur-to-mil", bind(&CCommandHandler::ConvertToMeale, this, _1) },
		{ "minimize-mil", bind(&CCommandHandler::Minimize, this, _1) },
		{ "minimize-mur", bind(&CCommandHandler::Minimize, this, _1) },
		{ "write-to-output-file", bind(&CCommandHandler::WriteOutputToFile, this, _1) },
		{ "determine", bind(&CCommandHandler::Determine, this, _1) },
	})
{

}

void CCommandHandler::HandleCommand()
{
	string cmd;
	getline(m_input, cmd);

	istringstream ss(cmd);
	string action;
	ss >> action;
	auto & it = m_actions.find(action);
	if (it != m_actions.end())
	{
		it->second(ss);
	}
}

void CCommandHandler::ConvertToMeale(istream & strm)
{
	string id;
	strm >> id;
	m_smProcessor.ConvertToMeale(m_smProcessor.Get(id));
}

void CCommandHandler::ConvertToMoore(istream & strm)
{
	string id;
	strm >> id;
	m_smProcessor.ConvertToMoore(m_smProcessor.Get(id));
}

void CCommandHandler::Determine(std::istream & strm)
{
	string id;
	strm >> id;
	m_smProcessor.Determine(m_smProcessor.Get(id));
}

void CCommandHandler::Minimize(std::istream & strm)
{
	string id;
	strm >> id;
	m_smProcessor.Minimize(m_smProcessor.Get(id));
}

void CCommandHandler::WriteOutputToFile(std::istream & strm)
{
	m_smProcessor.WriteToFile();
}
