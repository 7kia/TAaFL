#include "stdafx.h"
#include "RunnerTests.h"

BOOST_AUTO_TEST_SUITE(ApplicationTestModule)


const std::string PATH_RIGHT_DATA = "Right\\";

void CompareFiles(const std::string & first, const std::string & second)
{
	std::ifstream resultFile(first);
	std::ifstream rightResultFile(second);

	std::istream_iterator<char> iterResultFile(resultFile), endIter;
	std::istream_iterator<char> iterRightRsultFile(rightResultFile);

	BOOST_CHECK_EQUAL_COLLECTIONS(iterResultFile, endIter, iterRightRsultFile, endIter);
};

BOOST_AUTO_TEST_CASE(test_all_command)
{
	std::string nameOutputFile = "output.txt";

	CompareFiles(nameOutputFile, PATH_RIGHT_DATA + nameOutputFile);
}


BOOST_AUTO_TEST_SUITE_END()
