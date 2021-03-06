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
#include <vector>

constexpr int MAX_NUM_AFTER_POINT = 1e6;

struct Node {
	Node(): c(0), p(0), code("") { }
    Node(char c, double p): c(c), p(p), code("") { }
    bool operator<(const Node &rhs) const { return this->p < rhs.p; }

    char c;
    double p;
    std::string code;
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

void printNodes(std::multiset<Node> &n)
{
    std::for_each(n.crbegin(), n.crend(), [](const Node &it) { std::cout << it.c << '\t' << it.p << '\t' << it.code << '\n'; });
}

std::multiset<Node> calcNodes(const std::string &fileName)
{
    std::unordered_map<std::string, size_t> strEnc = calcStrEnc(fileName, 1);
	size_t totalEnc = 0;
	for (auto &it : strEnc)
		totalEnc += it.second;
	
	std::multiset<Node> n;
	std::transform(strEnc.cbegin(), strEnc.cend(), std::inserter(n, n.begin()),
		[&totalEnc](const std::pair<std::string, size_t> &elem) 
		{ 
			return Node(elem.first.front(), (double)elem.second / totalEnc);
		});
		
	return n;
}

void calcHuffmanCode(std::multiset<Node> &n, size_t base)
{
    if (n.size() == 1)
        return;
    
	std::vector<Node> firstFewN(base);

	for (auto curInd = 0; curInd < firstFewN.size(); ++curInd)
	{
    	firstFewN[curInd] = *n.begin();
    	n.erase(n.begin());
	}
    Node mergedN = Node('#', std::accumulate(firstFewN.cbegin(), firstFewN.cend(), 0.0, [](double pSum, const Node &N){ return pSum += N.p; }));
    n.insert(mergedN);

    calcHuffmanCode(n, base);

    auto mergedNIter = n.find(mergedN);

	for (auto curInd = 0; curInd < firstFewN.size(); ++curInd)
	{
		firstFewN[curInd].code = mergedNIter->code + char('0' + curInd);
		n.insert(firstFewN[curInd]);
	}
    n.erase(mergedNIter);
}

void huffmanCode(std::multiset<Node> &n, size_t base)
{
	size_t addedAmount = base == 2 ? 0 : base - 2 - (n.size() % (base - 1) + base - 3) % (base - 1);
	for (size_t iter = 0; iter < addedAmount; ++iter)
		n.insert(Node('#', 0.0));

	calcHuffmanCode(n, base);

	for (size_t iter = 0; iter < addedAmount; ++iter)
		n.erase(n.begin());
}

double calcEntropy(std::list<double> &p, size_t symbInRawAm)
{
	double H = 0.0;
	for (auto &it : p)
		H += it * std::log2(it);
	return -H / symbInRawAm;
}

double calcMediumLen(std::multiset<Node> &n)
{
    double L = 0.0;
    for (auto &it : n)
        L += it.p * it.code.length();
    return L;
}

void writeEncodedFile(std::multiset<Node> &n, const std::string fileName)
{
	std::unordered_map<char, std::string> codeDict;
	for (auto& it : n)
		codeDict[it.c] = it.code;

	std::ifstream in(fileName);
	std::ofstream out("res.txt");

	for(char inputChar; in.get(inputChar);)
		out << codeDict[inputChar];
}

void testFile(const std::string fileName)
{
	const std::string preparedFile = prepareFile(fileName);
    std::multiset<Node> n = calcNodes(preparedFile);

	size_t base = 3;

    huffmanCode(n, base);
	printNodes(n);

    double L = calcMediumLen(n);
    std::cout << "L: " << L << '\n';

	if (base == 2)
	{
    	std::list<double> p = calcMean(preparedFile, 5);
    	double H = calcEntropy(p, 5);
		std::cout << "H: " << H << '\n';
    	std::cout << "r: " << L - H << '\n';
	}

	writeEncodedFile(n, "test.txt");

	for (size_t symbInRawAm = 1; symbInRawAm <= 5; ++symbInRawAm)
	{
		std::list<double> p = calcMean("res.txt", symbInRawAm);
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