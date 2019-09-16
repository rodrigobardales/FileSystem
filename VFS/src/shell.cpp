#include "shell.h"

void Shell::run()
{
	cout << "Running" << endl;
	do
	{
		//system.testprint();
		cout << "\nTerminal// \t";
		cin.getline(op, 256);
		tokens = tokenizer.getTokens(op);
		if (tokens.empty())
		{
			continue;
		}

		if (tokens[0] == "createdisk")
		{
			createDisk(tokens[1], stoi(tokens[2]));
		}
		else if (tokens[0] == "mkdir")
		{
			mkdir(tokens[1]);
		}
		else if (tokens[0] == "cd")
		{
			cd(tokens[1]);
		}
		else if (tokens[0] == "ls")
		{
			ls();
		}
		else if (tokens[0] == "rm")
		{
			rm(tokens[1]);
		}
		else if (tokens[0] == "importfile")
		{
		}
		else if (tokens[0] == "exportfile")
		{
		}
		else
		{
			if (tokens[0] != "exit")
				cout << "unknown command" << endl;
		}

	} while (tokens[0] != "exit");
	system.save_metaentries();
}

void Shell::createDisk(string diskName, int sizeInInodes)
{

	char name[30];
	for (size_t i = 0; i < 30; i++)
	{
		name[i] = diskName[i];
	}

	system.createDisk(name, sizeInInodes);
}

void Shell::mkdir(string directoryName)
{
	char name[30];
	for (size_t i = 0; i < 30; i++)
	{
		name[i] = directoryName[i];
	}
	system.mkdir(name);
}

void Shell::ls() 
{
	system.ls();
}

void Shell::cd(string directoryPath)
{
	char name[30];
	for (size_t i = 0; i < 30; i++)
	{
		name[i] = directoryPath[i];
	}
	system.cd(name);
}

void Shell::rm(string path)
{
	char name[30];
	for (size_t i = 0; i < 30; i++)
	{
		name[i] = path[i];
	}
	system.rm(name);
}