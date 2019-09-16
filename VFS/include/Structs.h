#ifndef STRUCTS_H
#define STRUCTS_H

struct FileDate {
	int day;
	int mon;
	int year;
};

enum Filetype {
	FILETYPE, ARCHIVETYPE
};

struct Metadata {
	char name[30];
    unsigned int size;
    FileDate date;
    char owner[30];
    unsigned int entries;
    unsigned int bitmap_size;
    unsigned int bloquesdirectos;
	int currentEntries, available_entries;
};

struct FileEntry {
	char fileName[30] = "/";
	int code = -1;
	Filetype FileType; //d: directorio f:file
	FileDate date;
	int father = -1;
	int firstson = -1;
	int rightbrother = -1;
	int lastson = -1;
	bool used = false;
	bool current = false;

};

struct DataBlock_Direct {
	char data[4096];
};

struct DataBlock_I1 {
	unsigned int data[16];
};
struct DataBlock_I2 {
	unsigned int data1[32];
};
struct DataBlock_I3 {
	unsigned int data2[64];

};

struct bitMap {
	int entries;
	char * bitmap;
	char * bitmapI1;
};

#endif