#ifndef SHELL_H_
#define SHELL_H_

#include <string>
#include <fstream>
#include <iostream>
#include <FileSystem.h>

using namespace std;

#include <vector>
class Tokenizer
{
  public:
	unsigned int current;
	vector<string> tokens;
	string buffer;
	Tokenizer(){}
	vector<string> getTokens(string text)
	{
		tokens.clear();
		buffer.clear();
		current = 0;
		do
		{
			if (text[current] == ' ')
			{
				tokens.push_back(buffer);
				buffer.clear();
				current++;
				continue;
			}
			buffer += text[current];
			current++;
		} while (current != text.size());
		tokens.push_back(buffer);
		return tokens;
	}
};

class Shell
{

public:

	void run();
	void createDisk(string diskName, int sizeMB);
	void mkdir(string directoryName);
	void cd(string directoryPath);
	void ls();
	void rm(string path);
	void importFile(string path);
	void exportFile(string path);

	Shell(FileSystem& filesystem) : system(filesystem) {
	}

	char op[256];
	Tokenizer tokenizer;
	vector<string> tokens;

	FileSystem system;
};

#endif