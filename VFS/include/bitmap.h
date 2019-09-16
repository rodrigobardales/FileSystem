#ifndef BITMAP_H_
#define BITMAP_H_

#include <iostream>

using namespace std;


class Bitmap {

  public:
    void printBitMap();
    void setOn(int);
    void setOff(int);

    char* load();
    void write(char*);

    Bitmap(int size): size(size) {
        bitMap = new char[size/8];

        for(int i = 0; i < size/8; i++)
        {
            bitMap[i] = 0;
        }
    }

  private:
    char* bitMap;
    int size;
};

#endif