#include <iostream>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <numeric>
#include <utility>
#include <fstream>
#include <cmath>
#include <cctype>

constexpr int MAX_NUM_AFTER_POINT = 1e6;

const std::string prepareFile(const std::string &fileName)
{
	const std::string resultFileName = "test.txt";
	
	std::ifstream in(fileName);
	std::ofstream out(resultFileName);
	
	std::string line;
	while(std::getline(in, line))
	{
		for (auto& c : line)
		{
			if (ispunct(c))
				c = '.';
			else if (isupper(c))
				tolower(c);
			out << c;
		}
	}
	
	in.close();
	out.close();
	
	return resultFileName;
}

std::list<double> calcMean(const std::string &fileName, const size_t symbInRaw)
{
	std::ifstream in(fileName);
	char inputChar;
	
	std::list<char> raw;
	for (size_t curSymbInd = 0; curSymbInd < symbInRaw - 1; ++curSymbInd)
	{
		in >> inputChar;
		raw.push_back(inputChar);
	}
	
	std::unordered_map<std::string, size_t> strEnc;
	while(in >> inputChar)
	{
		raw.push_back(inputChar);
		const std::string rawStr(raw.cbegin(), raw.cend());
		
		++strEnc[rawStr];
		
		raw.pop_front();
	}
	
	in.close();
	
	size_t totalEnc = 0;
	for (auto &it : strEnc)
		totalEnc += it.second;
	
	std::list<double> p;
	std::transform(strEnc.cbegin(), strEnc.cend(), std::back_inserter(p),
		[&totalEnc](const std::pair<std::string, size_t> &elem) 
		{ 
			return (double)elem.second / totalEnc;
		});
		
	return p;
}

double calcEntropy(std::list<double> &p, size_t symbInRawAm)
{
	double H = 0.0;
	for (auto &it : p)
		H += it * std::log2(it);
	return -H / symbInRawAm;
}

void testFile(const std::string fileName)
{
	const std::string preparedFile = prepareFile(fileName);
	
	for (size_t symbInRawAm = 1; symbInRawAm <= 5; ++symbInRawAm)
	{
		std::list<double> p = calcMean(preparedFile, symbInRawAm);
		std::cout << "For " << symbInRawAm << 
			" symbols in a raw entropy equals " << calcEntropy(p, symbInRawAm) << '\n';
	}
	std::cout << '\n';
}

int main()
{
	srand(time(0));
	
	testFile("Gardens-of-the-Moon.txt");
	
	return 0;
}