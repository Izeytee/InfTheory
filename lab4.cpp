#include <iostream>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <numeric>
#include <utility>
#include <fstream>
#include <cmath>
#include <cctype>
#include <set>

constexpr int MAX_NUM_AFTER_POINT = 1e6;

struct Node {
    Node(char c, double p): c(c), p(p), binCode("") { }
    bool operator<(const Node &rhs) const { return this->p < rhs.p; }

    char c;
    double p;
    std::string binCode;
};

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
			if(isalpha(c) || c == ' ')
				c = tolower(c);
            else
                continue;
			out << c;
		}
	}
	
	in.close();
	out.close();
	
	return resultFileName;
}

std::unordered_map<std::string, size_t> calcStrEnc(const std::string &fileName, const size_t symbInRaw)
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
	while(in.get(inputChar))
	{
		raw.push_back(inputChar);
		const std::string rawStr(raw.cbegin(), raw.cend());
		
		++strEnc[rawStr];
		
		raw.pop_front();
	}
	in.close();
	
    return strEnc;
}

std::list<double> calcMean(const std::string &fileName, const size_t symbInRaw)
{
    std::unordered_map<std::string, size_t> strEnc = calcStrEnc(fileName, symbInRaw);
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

void printNodes(std::set<Node> &n)
{
    for (auto& it : n)
        std::cout << it.c << ' ' << it.p << ' ' << it.binCode << '\n';
}

std::set<Node> calcNodes(const std::string &fileName)
{
    std::unordered_map<std::string, size_t> strEnc = calcStrEnc(fileName, 1);
	size_t totalEnc = 0;
	for (auto &it : strEnc)
		totalEnc += it.second;
	
	std::set<Node> n;
	std::transform(strEnc.cbegin(), strEnc.cend(), std::inserter(n, n.begin()),
		[&totalEnc](const std::pair<std::string, size_t> &elem) 
		{ 
			return Node(elem.first.front(), (double)elem.second / totalEnc);
		});
		
	return n;
}

void calcHuffmanBinCode(std::set<Node> &n)
{
    if (n.size() == 1)
        return;
    
    Node firstN = *n.begin();
    n.erase(firstN);
    Node secondN = *n.begin();
    n.erase(secondN);
    Node mergedN = Node('#', firstN.p + secondN.p);
    n.insert(mergedN);

    calcHuffmanBinCode(n);

    mergedN = *n.find(mergedN);
    firstN.binCode = mergedN.binCode + '0';
    secondN.binCode = mergedN.binCode + '1';
    n.erase(mergedN);
    n.insert(firstN);
    n.insert(secondN);
}

double calcEntropy(std::list<double> &p, size_t symbInRawAm)
{
	double H = 0.0;
	for (auto &it : p)
		H += it * std::log2(it);
	return -H / symbInRawAm;
}

double calcMediumLen(std::set<Node> &n)
{
    double L = 0.0;
    for (auto &it : n)
        L += it.p * it.binCode.length();
    return L;
}

void testFile(const std::string fileName)
{
	const std::string preparedFile = prepareFile(fileName);
    std::set<Node> n = calcNodes(preparedFile);
    calcHuffmanBinCode(n);
	printNodes(n);

    double L = calcMediumLen(n);
    std::cout << "L: " << L << '\n';

    std::list<double> p = calcMean(preparedFile, 5);
    double H = calcEntropy(p, 5);

    std::cout << "r: " << L - H << '\n';
}

int main()
{
	srand(time(0));
	
	testFile("Gardens-of-the-Moon.txt");
	
	return 0;
}