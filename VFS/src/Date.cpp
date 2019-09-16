#include "Date.h"

Date::Date(){}

FileDate Date::getdate()
{
    FileDate filedate;

    time_t now = time(0);

    tm *ltm = localtime(&now);

    filedate.year = 1900 + ltm->tm_year;
    filedate.mon = 1 + ltm->tm_mon;
    filedate.day = ltm->tm_mday;

    return filedate;
}