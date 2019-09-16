#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <iostream>
#include <string.h>
#include <fstream>
#include "Structs.h"
#include "bitmap.h"
#include "Date.h"
#include <cctype>
#include <cstdio>

using namespace std;

class FileSystem
{
private:
    Date filedate;
    //BitMap bitmapclass;
    string file = "Disk.txt";
public:
    int currentEntry = 0, entries;

    FileSystem();
    //disk funcs
    void createDisk(char[20], int);
    void mkdir(char[30]);
    void cd(char[30]);
    void cdback();
    void ls();
    void rm(char[30]);
    void importfile();
    void exportfile();

    //tool funcs
    int get_currentfathercode();
    FileEntry get_currentfather();
    FileEntry get_current();
    int getcurrentcode();
    FileEntry get_currentEntry();

    void updateMeta();
    bool CheckSpace();
    void downMeta();

    string tolower(string);

    void load_metaentries();
    void save_metaentries();

    void gotoroot();

    FileEntry get_entry(int);
    //FileEntry get_Entry(char[30]);

    void modifyFather(FileEntry, int);
    void modifyBrother(FileEntry, int);

    //testing 
    void testprint();
    bool find(char[30]);
};

#endif