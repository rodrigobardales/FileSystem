#include <iostream>
#include "FileSystem.h"
#include "Date.h"
#include "shell.h"

int main()
{
    
    FileSystem system;
    cout << "from memory loads: current entry" << system.currentEntry << " available" << system.entries;
    system.load_metaentries();
    system.gotoroot();
    Shell shell(system);
    shell.run();
    // system.createDisk("disk", 3);
    // system.mkdir("rodrigo");
    // system.testprint();
    // system.mkdir("nimer");
    // system.testprint();
    //system.cd("rodrigo");
    // system.testprint();
}