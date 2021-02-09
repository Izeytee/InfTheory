#include <iostream>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <numeric>
#include <utility>
#include <fstream>
#include <cmath>

constexpr int MAX_NUM_AFTER_POINT = 1e6;

void initFile(const std::unordered_map<char, double> &alph, 
			  const std::string &fileName, const size_t charsAm)
{
	std::list<std::pair<char, int>> charsProbXAfterPoint;
	std::transform(alph.cbegin(), alph.cend(), std::back_inserter(charsProbXAfterPoint),
		[](const std::pair<char, double> &elem) 
		{ 
			return std::make_pair(elem.first, elem.second * MAX_NUM_AFTER_POINT);
		});
		
	std::ofstream out(fileName);
	for (size_t curCharInd = 0; curCharInd < charsAm; ++curCharInd)
	{
		int propPointsLeft = rand() % MAX_NUM_AFTER_POINT;
		for (auto it = charsProbXAfterPoint.cbegin();
			it != charsProbXAfterPoint.cend(); ++it)
			if ((propPointsLeft -= it->second) <= 0)
			{
				out << it->first;
				break;
			}
	}
	out.close();
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

void testAlpEntr(const std::unordered_map<char, double> &alph, 
				const std::string fileName, const size_t charsAm)
{
	initFile(alph, fileName, charsAm);
	/*
	std::cout << "Alphabet is: \n";
	for (auto &it : alph)
		std::cout << '{' << it.first << ", " << it.second << '}';
	std::cout << '\n';
	*/
	for (size_t symbInRawAm = 1; symbInRawAm <= 2; ++symbInRawAm)
	{
		std::list<double> p = calcMean(fileName, symbInRawAm);
		std::cout << "probabilities: ";
		for (auto& it : p)
			std::cout << it << ' ';
		std::cout << '\n';
		std::cout << "For " << symbInRawAm << 
			" symbols in a raw entropy equals " << calcEntropy(p, symbInRawAm) << '\n';
	}
	std::cout << '\n';
}

int main()
{
	srand(time(0));
	
	std::unordered_map<char, double> 
		alphEq{{'a', 0.25}, {'b', 0.25}, {'c', 0.25}, {'d', 0.25}},
		alphDif{{'a', 0.33}, {'b', 0.13}, {'c', 0.07}, {'d', 0.47}};
	
	testAlpEntr(alphEq, "alphEq.txt", 50000);
	testAlpEntr(alphDif, "alphDif.txt", 50000);
	
	
	return 0;
}