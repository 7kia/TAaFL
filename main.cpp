#include "stdafx.h"
#include "main.h"
#include "CommandHandler.h"

using namespace std;

void CheckParametrs(int argc)
{
	if (argc != AMOUNT_ARGUMENTS)
	{
		throw invalid_argument(MESSAGE_INCORRECT_AMOUNT_ARGUMENTS + to_string(AMOUNT_ARGUMENTS));
	}
}

int main(int argc, char* argv[])
{
	try
	{
		CheckParametrs(argc);

		std::ifstream inputFile(argv[1]);
		CStateMachineProcessor stateMachineProcesor(argv[2], argv[3]);
		CCommandHandler handler(stateMachineProcesor, inputFile);

		while (!inputFile.eof() && !inputFile.fail())
		{
			handler.HandleCommand();
		}
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
		return 1;
	}

	return 0;
}

