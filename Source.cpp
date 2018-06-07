#include <iostream>
#include <fstream>
#include <unordered_map>
#include <climits>
#include <string>
#include <thread>
#include <queue>
#include <ctime>
using namespace std;

const int UniqSymb = 256;     //size of char

class TNode
{
public:
	const int Frequency;

	virtual ~TNode() {}

protected:
	TNode(int Frequency) : Frequency(Frequency) {}
};

class BinaryNode : public TNode
{
public:
	TNode *const Left;
	TNode *const Right;

	BinaryNode(TNode* LeftTempNode, TNode* RightTempNode) :
		TNode(LeftTempNode->Frequency + RightTempNode->Frequency), Left(LeftTempNode), Right(RightTempNode) {}

	~BinaryNode()
	{
		delete Left;
		delete Right;
	}
};

class ValNode : public TNode
{
public:
	const char Symbol;

	ValNode(int Frequency, char Symbol) : TNode(Frequency), Symbol(Symbol) {}
};

struct NodeCompare
{
	bool operator()(const TNode* LeftTnode, const TNode* RightTnode) const
	{
		return LeftTnode->Frequency > RightTnode->Frequency;
	}
};

class HuffmanCoder
{
public:
	void AddToArchive(string InputName, string ArchiveName);
	void ConvertToArchive();
	HuffmanCoder();

private:
	typedef unordered_map<char, string> HuffCodeMap;
	bool is_written = false;
	thread connectionThread;
	vector<thread> Thread_vector;
	TNode* BuildTree(int(&Frequencies)[UniqSymb]);
	void BinaryCodes(const TNode* Node, const string& Prefix, HuffCodeMap &CodeMap);
	void CompressToArchive(string InputName,string ArchiveName);
};

void HuffmanCoder::CompressToArchive(string InputName, string ArchiveName)
{
	ofstream ArchiveFile;
	if (is_written) {
		ArchiveFile.open(ArchiveName, ios_base::app);
	}
	else {
		ArchiveFile.open(ArchiveName, ios_base::out);
		is_written = true;
}
	ifstream InputFile;
	InputFile.open(InputName, ios_base::in);
	if (InputFile.is_open())
	{
		int frequency[UniqSymb] = { 0 };
		string InputBuffer, TotalBuffer = "";
		
		while (getline(InputFile, InputBuffer))
		{
			TotalBuffer += InputBuffer;
			
			for (int i = 0; i < InputBuffer.length(); i++)
			{
				frequency[InputBuffer[i]]++;
				/*cout << InputBuffer[i];
				cout << (int)InputBuffer[i];
				cout<< frequency[InputBuffer[i]]<<" ";*/

			}
				
		}
		/*for (int i = 0; i < InputBuffer.length(); i++)
		{
			cout << frequency[InputBuffer[i]] ;
		} cout<< endl;*/

		TNode* Root = BuildTree(frequency);
		HuffCodeMap CodeMap;
		CodeMap.clear();
		BinaryCodes(Root, string(), CodeMap);
		for (auto Symbol : TotalBuffer)
			ArchiveFile << CodeMap[Symbol];
	}
	else { cout << "File is not found\n"; }
}

void HuffmanCoder::AddToArchive(string InputName, string ArchiveName)
{
	Thread_vector.push_back(thread(&HuffmanCoder::CompressToArchive, this, InputName, ArchiveName));
}

void HuffmanCoder::ConvertToArchive()
{
	while (Thread_vector.size())
	{
		if (Thread_vector.back().joinable())
		{
			Thread_vector.back().join();
			Thread_vector.pop_back();
		}
	}
}

HuffmanCoder::HuffmanCoder()
{
	Thread_vector.clear();
	is_written = false;
}

TNode* HuffmanCoder::BuildTree(int  (&Frequencies) [UniqSymb])
{
	priority_queue<TNode*,vector<TNode*>, NodeCompare> Trees;

	for (int i = 0; i < UniqSymb; ++i)
	{
		if (Frequencies[i] != 0)
			Trees.push(new ValNode(Frequencies[i], (char)i));
	}
	while (Trees.size() > 1)
	{
		TNode* RightChild = Trees.top();
		Trees.pop();

		TNode* LeftChild = Trees.top();
		Trees.pop();

		TNode* Parent = new BinaryNode(RightChild, LeftChild);
		Trees.push(Parent);
	}
	return (Trees.size() ? Trees.top() : nullptr);
}

void HuffmanCoder::BinaryCodes(const TNode* Node, const string& Prefix, HuffCodeMap &CodeMap)
{
	if (!Node)
		return;

	if (const ValNode* Leaf = dynamic_cast<const ValNode*>(Node))
		CodeMap[Leaf->Symbol] = Prefix;
	else if (const BinaryNode* Internal = dynamic_cast<const BinaryNode*>(Node))
	{
		string LeftPrefix = Prefix;
		LeftPrefix += "0";
		BinaryCodes(Internal->Left, LeftPrefix, CodeMap);

		string RightPrefix = Prefix;
		RightPrefix += "1";
		BinaryCodes(Internal->Right, RightPrefix, CodeMap);
	}
}

int main()
{
	HuffmanCoder Compressor = HuffmanCoder();    

	cout << "Please enter file to write archive code:\n";
	string OutputFile;
	cin >> OutputFile;

	unsigned int start_time = clock(); 

	while (true)
	{
		cout << "Please enter file needing to code or press Z to exit:\n";
		string InputFile;
		cin >> InputFile;
		if (InputFile == "z" || InputFile == "Z")
			break;
		else Compressor.AddToArchive(InputFile, OutputFile);
	}

	Compressor.ConvertToArchive();

	unsigned int end_time = clock(); 
	unsigned int search_time = end_time - start_time; 
	cout << "Runtime=" << search_time << endl;
	system("pause");
	return 0;
}