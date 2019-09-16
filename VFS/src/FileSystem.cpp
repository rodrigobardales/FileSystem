#include "FileSystem.h"

FileSystem::FileSystem() {}

/*
12 bloques directos
bloque de 1 nivel apunta a 16 bloques
bloque de 2 nivel tiene 32 apuntadores a bloques de 1 nivel que apuntan a 16 directos
*/

void FileSystem::createDisk(char name[20], int entries)
{
    ofstream write(file, ios::out | ios::app | ios::binary);

    if (!write)
    {
        cout << "Bad Disk\n";
        write.close();
        return;
    }

    this->entries = entries;

    //get blocks
    int directblocks = 33308 * entries;
    int firstlevel = 2081 * entries;
    int secondlevel = 65 * entries;
    int thirdlevel = 1 * entries;

    //create Metadata
    Metadata meta;
    strcpy(meta.name, name);
    meta.entries = entries;
    meta.bloquesdirectos = directblocks;
    meta.bitmap_size = 4096;
    meta.date.day = filedate.getdate().day;
    meta.date.mon = filedate.getdate().mon;
    meta.date.year = filedate.getdate().year;
    strcpy(meta.owner, "rodrigo");
    meta.size = 0;
    meta.currentEntries = currentEntry;

    write.write(reinterpret_cast<const char *>(&meta), sizeof(Metadata));

    //create Bitmap
    Bitmap bitmap(directblocks);

    cout << "Procesing... \n";

    //File Entry
    FileEntry root;
    strcpy(root.fileName, "root");
    root.code = 0;
    root.date.day = filedate.getdate().day;
    root.date.mon = filedate.getdate().mon;
    root.date.year = filedate.getdate().year;
    root.FileType = ARCHIVETYPE;
    root.father = -1;
    root.firstson = -1;
    root.rightbrother = -1;
    root.lastson = -1;
    root.current = true;
    root.used = true;

    write.write(reinterpret_cast<const char *>(&root), sizeof(FileEntry));

    //write entries
    cout << "Procesing... \n";
    FileEntry newentry;
    for (int i = 0; i < entries - 1; i++)
    {
        write.write(reinterpret_cast<const char *>(&newentry), sizeof(FileEntry));
    }

    DataBlock_Direct tmpdirect;
    // DataBlock_I1 tmpdirect1;
    // DataBlock_I2 tmpdirect2;
    // DataBlock_I3 tmpdirect3;

    cout << "Procesing... \n";
    for (int i = 0; i < directblocks; i++)
    {
        write.write(reinterpret_cast<const char *>(&tmpdirect), sizeof(DataBlock_Direct));
    }
    // cout << "Procesing... \n";
    // for (int i = 0; i < firstlevel; i++)
    // {
    //     write.write(reinterpret_cast<const char *>(&tmpdirect1), sizeof(DataBlock_I1));
    // }
    // cout << "Procesing... \n";
    // for (int i = 0; i < secondlevel; i++)
    // {
    //     write.write(reinterpret_cast<const char *>(&tmpdirect2), sizeof(DataBlock_I2));
    // }
    // cout << "Procesing... \n";
    // for (int i = 0; i < thirdlevel; i++)
    // {
    //     write.write(reinterpret_cast<const char *>(&tmpdirect3), sizeof(DataBlock_I3));
    // }

    write.close();
    updateMeta();

    cout << "Disk created\n";
}

void FileSystem::mkdir(char newname[30])
{
    cout << "entra a mkdir\n";
    //checks for space
    cout << "space: " << CheckSpace();
    if (CheckSpace() == false)
    {
        cerr << "NOT ENOUGH SPACE ON DRIVE \n";
        return;
    }

    fstream write(file, ios::in | ios::out | ios::binary);
    if (!write)
    {
        cerr << "File Error" << endl;
        write.close();
        return;
    }

    FileEntry new_entry;

    //define type of new entry
    new_entry.FileType = ARCHIVETYPE;

    //ask for file name
    strcpy(new_entry.fileName, newname);

    //entry's code
    new_entry.code = currentEntry;

    //update metadata
    updateMeta();

    //newfile date
    new_entry.date.day = filedate.getdate().day;
    new_entry.date.mon = filedate.getdate().mon;
    new_entry.date.year = filedate.getdate().year;

    //newEntry father
    new_entry.father = get_currentfathercode();

    //sons and brothers
    new_entry.firstson = -1;
    new_entry.rightbrother = -1;
    new_entry.lastson = -1;

    //using and current
    new_entry.used = true;
    new_entry.current = false;

    FileEntry w;
    int cont = 0;
    write.seekg(sizeof(Metadata), ios::beg);
    write.read(reinterpret_cast<char *>(&w), sizeof(FileEntry));
    while (!write.eof())
    {
        if (w.used == false)
        {
            //copy new_entry to w to write
            w = new_entry;

            //get position to update
            int current_pos = write.tellp();
            write.seekp(current_pos - sizeof(FileEntry), ios::beg);

            write.write(reinterpret_cast<char *>(&w), sizeof(FileEntry));
            write.close();
            cout << "Archive created \n";
            FileEntry f = get_current();
            modifyFather(f, w.code);
            //FileEntry brother = get_entry(get_currentEntry().firstson);
            //modifyBrother(brother, w.code);
            return;
        }
        write.read(reinterpret_cast<char *>(&w), sizeof(FileEntry));
        cont++;
    }
    write.close();
    cout << "Archive failed to create \n";
}

//return current code
int FileSystem::get_currentfathercode()
{
    ifstream fileread(file, ios::in | ios::binary);

    if (!fileread)
    {
        cerr << "File Error" << endl;
        fileread.close();
        return -1;
    }

    FileEntry reading;
    int cont = 0;
    fileread.seekg(0 + sizeof(Metadata), ios::beg);
    fileread.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
    while (cont <= entries)
    {
        if (reading.current == true)
        {
            cout << "father encontrado es: " << reading.fileName << " codigo: " << reading.code << endl;
            fileread.close();
            return reading.code;
        }
        fileread.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
        cont++;
    }
    fileread.close();
    return -1;
}

FileEntry FileSystem::get_currentEntry()
{
    ifstream fileread(file, ios::in | ios::binary);
    FileEntry reading;

    if (!fileread)
    {
        cout << "disk error \n";
        fileread.close();
        return reading;
    }

    fileread.seekg(0 + sizeof(Metadata), ios::beg);
    fileread.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
    int cont = 0;
    while (cont < entries)
    {
        if (reading.current == true)
        {
            fileread.close();
            return reading;
        }
        fileread.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
        cont++;
    }
    fileread.close();
    return reading;
}

FileEntry FileSystem::get_current()
{
    ifstream fileread(file, ios::in | ios::binary);
    FileEntry reading;

    if (!fileread)
    {
        cout << "disk error \n";
        fileread.close();
        return reading;
    }
    int cont = 0;
    fileread.seekg(0 + sizeof(Metadata), ios::beg);
    fileread.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
    while (cont < entries)
    {
        if (reading.current == true)
        {
            fileread.close();
            return reading;
        }
        fileread.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
        cont++;
    }
    fileread.close();
    return reading;
}
//updates meta currentEntry
void FileSystem::updateMeta()
{
    currentEntry++;
    fstream update(file, ios::in | ios::out | ios::binary);
    if (!update)
    {
        cout << "ERROR WITH FILE \n";
        update.close();
        return;
    }

    Metadata meta;
    update.seekg(0, ios::beg);
    update.read(reinterpret_cast<char *>(&meta), sizeof(Metadata));
    meta.currentEntries = currentEntry;
    int current_pos = update.tellp();
    update.seekp(current_pos - sizeof(Metadata), ios::beg);
    update.write(reinterpret_cast<char *>(&meta), sizeof(Metadata));
    update.close();
}

void FileSystem::downMeta()
{
    currentEntry--;
    fstream update(file, ios::in | ios::out | ios::binary);
    if (!update)
    {
        cout << "ERROR WITH FILE \n";
        update.close();
        return;
    }

    Metadata meta;
    update.seekg(0, ios::beg);
    update.read(reinterpret_cast<char *>(&meta), sizeof(Metadata));
    meta.currentEntries = currentEntry;
    int current_pos = update.tellp();
    update.seekp(current_pos - sizeof(Metadata), ios::beg);
    update.write(reinterpret_cast<char *>(&meta), sizeof(Metadata));
    update.close();
}

//checks for available space on disk
bool FileSystem::CheckSpace()
{
    ifstream read(file, ios::in | ios::binary);
    if (!read)
    {
        cout << "ERROR WITH FILE \n";
        read.close();
        return false;
    }

    FileEntry entry;
    Metadata meta;

    //get amount of entries
    read.read(reinterpret_cast<char *>(&meta), sizeof(Metadata));
    int entries = meta.entries;

    //get available entries

    //if there are more entries than available
    if (entries > meta.currentEntries)
    {
        read.close();
        return true;
    }
    read.close();
    return false;
}

//changes a string to lower case
string FileSystem::tolower(string lowering)
{
    transform(lowering.begin(), lowering.end(), lowering.begin(), ::tolower);
    return lowering;
}

//list archives in an used entry
void FileSystem::ls()
{
    ifstream reading(file, ios::in | ios::binary);
    if (!reading)
    {
        cerr << "File Error" << endl;
        reading.close();
        return;
    }
    int currentfather = get_currentfathercode();
    FileEntry entry;
    int cont = 0, sons = 0;
    reading.seekg(0 + sizeof(Metadata), ios::beg);
    reading.read(reinterpret_cast<char *>(&entry), sizeof(FileEntry));
    while (cont < this->entries)
    {
        if (entry.father == currentfather && entry.used == true)
        {
            cout << "Name: \t";
            for (size_t i = 0; i < 30; i++)
            {
                cout << entry.fileName[i];
            }
            cout << "\t code:\t" << entry.code << "\t File type:\t" << entry.FileType << endl;
            sons++;
        }
        reading.read(reinterpret_cast<char *>(&entry), sizeof(FileEntry));
        cont++;
    }
    reading.close();
    if (sons == 0)
        cout << "Archive is empty" << endl;
}

FileEntry FileSystem::get_entry(int getting)
{
    ifstream fileread(file, ios::in | ios::binary);
    FileEntry reading;
    if (!fileread)
    {
        cout << "disk error \n";
        fileread.close();
        return reading;
    }
    int cont = 0;
    fileread.seekg(0 + sizeof(Metadata), ios::beg);
    fileread.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
    while (cont < entries)
    {
        if (reading.code == getting)
        {
            fileread.close();
            return reading;
        }
        fileread.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
        cont++;
    }
    fileread.close();
    return reading;
}

void FileSystem::testprint()
{
    ifstream fileread(file, ios::in | ios::binary);

    if (!fileread)
    {
        cout << "disk error \n";
        fileread.close();
        return;
    }

    FileEntry reading;

    fileread.seekg(sizeof(Metadata), ios::cur);
    fileread.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
    int cont = 0;
    while (cont <= entries)
    {
        if (reading.used == true)
        {
            cout << endl
                 << endl;
            cout << "used" << reading.used << endl;
            for (size_t i = 0; i < 30; i++)
            {
                cout << reading.fileName[i];
            }
            cout << endl
                 << "first son: " << reading.firstson << "\n right brother" << reading.rightbrother << "\ncode: " << reading.code << "\n current: " << reading.current
                 << "\nfather" << reading.father << endl;
        }
        fileread.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
        cont++;
    }
    fileread.close();
    cout << "termina de imprimir\n";
}

//searches for an entry
bool FileSystem::find(char find[30])
{
    ifstream fileread(file, ios::in | ios::binary);
    if (!fileread)
    {
        cout << "disk error \n";
        fileread.close();
        return false;
    }

    FileEntry reading;

    fileread.seekg(0 + sizeof(Metadata), ios::beg);
    fileread.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));

    int cont = 0;

    while (cont <= entries)
    {
        if (strcmp(find, reading.fileName) == 0)
        {
            fileread.close();
            return true;
        }
        fileread.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
        cont++;
    }
    fileread.close();
    return false;
}

//changes directory
void FileSystem::cd(char moving[30])
{
    fstream changing(file, ios::in | ios::out | ios::binary);
    if (!changing)
    {
        cout << "ERROR WITH FILE \n";
        changing.close();
        return;
    }

    FileEntry active;
    FileEntry activating;
    int currentfather;

    if (strcmp(moving, "..") == 0)
    {
        cdback();
        return;
    }

    if (find(moving) == false)
    {
        cout << "Item not found \n";
        return;
    }
    FileEntry deleted;
    changing.seekg(0 + sizeof(Metadata), ios::cur);
    changing.read(reinterpret_cast<char *>(&deleted), sizeof(FileEntry));
    int cont = 0;
    while (cont << entries)
    {
        if (strcmp(moving, deleted.fileName) == 0 && deleted.used == true)
        {
            changing.close();
            return;
        }
        cont++;
        changing.read(reinterpret_cast<char *>(&deleted), sizeof(FileEntry));
    }

    changing.seekg(0 + sizeof(Metadata), ios::cur);
    changing.read(reinterpret_cast<char *>(&active), sizeof(FileEntry));
    while (!changing.eof())
    {
        if (active.current == true)
        {
            currentfather = get_currentfathercode();
            active.current = false;
            //get position to update
            int current_pos = changing.tellp();
            changing.seekp(current_pos - sizeof(FileEntry), ios::beg);
            changing.write(reinterpret_cast<char *>(&active), sizeof(FileEntry));

            changing.seekg(0 + sizeof(Metadata), ios::beg);
            changing.read(reinterpret_cast<char *>(&activating), sizeof(FileEntry));
            while (!changing.eof())
            {
                if (strcmp(moving, activating.fileName) == 0 && activating.father == currentfather && activating.used == true)
                {
                    activating.current = true;
                    int current_pos = changing.tellp();
                    changing.seekp(current_pos - sizeof(FileEntry), ios::beg);
                    changing.write(reinterpret_cast<char *>(&activating), sizeof(FileEntry));
                    changing.close();
                    cout << "Directory changed\n";
                    return;
                }
                changing.read(reinterpret_cast<char *>(&activating), sizeof(FileEntry));
            }
            changing.close();
            return;
        }
        changing.read(reinterpret_cast<char *>(&active), sizeof(FileEntry));
    }
    changing.close();
    cout << "Failed to change directory \n";
}

void FileSystem::cdback()
{
    char movingto[30];
    strcpy(movingto, get_entry(get_current().father).fileName);

    fstream changing(file, ios::in | ios::out | ios::binary);
    if (!changing)
    {
        cout << "ERROR WITH FILE \n";
        changing.close();
        return;
    }

    FileEntry deactivating;
    FileEntry activating;
    int cont = 0;

    if (find(movingto) == false)
    {
        cout << "Item not found \n";
        return;
    }

    if (get_current().father == -1 || strcmp(get_current().fileName, "root") == 0)
    {
        cout << "Can't go further back, already in root \n";
        return;
    }

    changing.seekg(0 + sizeof(Metadata), ios::cur);
    changing.read(reinterpret_cast<char *>(&deactivating), sizeof(FileEntry));
    while (cont < entries)
    {
        if (deactivating.current == true)
        {
            deactivating.current = false;
            int current_pos = changing.tellp();
            changing.seekp(current_pos - sizeof(FileEntry), ios::beg);
            changing.write(reinterpret_cast<char *>(&deactivating), sizeof(FileEntry));

            changing.seekg(0 + sizeof(Metadata), ios::beg);
            changing.read(reinterpret_cast<char *>(&activating), sizeof(FileEntry));
            int count = 0;
            while (count < entries)
            {
                if (strcmp(activating.fileName, movingto) == 0 && activating.code == deactivating.father && activating.used == true)
                {
                    activating.current = true;
                    int current_pos = changing.tellp();
                    changing.seekp(current_pos - sizeof(FileEntry), ios::beg);
                    changing.write(reinterpret_cast<char *>(&activating), sizeof(FileEntry));
                    changing.close();
                    cout << "Directory changed\n";
                    return;
                }
                changing.read(reinterpret_cast<char *>(&activating), sizeof(FileEntry));
                count++;
            }
        }
        changing.read(reinterpret_cast<char *>(&deactivating), sizeof(FileEntry));
        cont++;
    }
    changing.close();
    cerr << "Failed to change directory \n";
}

//saves to meta the amount of used entries
void FileSystem::save_metaentries()
{
    fstream update(file, ios::in | ios::out | ios::binary);
    if (!update)
    {
        cout << "ERROR WITH FILE \n";
        update.close();
        return;
    }

    Metadata meta;
    int current_pos = update.tellp();
    update.seekp(current_pos - sizeof(Metadata), ios::beg);
    update.read(reinterpret_cast<char *>(&meta), sizeof(Metadata));
    meta.currentEntries = currentEntry;
    meta.entries = entries;
    update.write(reinterpret_cast<char *>(&meta), sizeof(Metadata));
    update.close();
}

//loads current amount of used entries
void FileSystem::load_metaentries()
{
    fstream update(file, ios::in | ios::out | ios::binary);
    if (!update)
    {
        cout << "ERROR WITH FILE \n";
        update.close();
        return;
    }

    Metadata meta;
    //int current_pos = update.tellg();
    update.seekg(0, ios::beg);
    update.read(reinterpret_cast<char *>(&meta), sizeof(Metadata));
    currentEntry = meta.currentEntries;
    entries = meta.entries;
    cout << "Loaded from metadata: \ncurrunet entries: " << meta.currentEntries
         << " available entries: " << meta.entries << endl;
    update.close();
}

//gets system back to root file
void FileSystem::gotoroot()
{
    fstream changing(file, ios::in | ios::out | ios::binary);
    if (!changing)
    {
        cout << "ERROR WITH FILE \n";
        changing.close();
        return;
    }

    FileEntry active;
    FileEntry activating;

    int cont = 0;

    int currentfather;
    changing.seekg(0 + sizeof(Metadata), ios::cur);
    changing.read(reinterpret_cast<char *>(&active), sizeof(FileEntry));
    while (cont <= entries)
    {
        if (active.current == true)
        {
            currentfather = get_currentfathercode();
            active.current = false;
            //get position to update
            int current_pos = changing.tellp();
            changing.seekp(current_pos - sizeof(FileEntry), ios::beg);
            changing.write(reinterpret_cast<char *>(&active), sizeof(FileEntry));

            changing.seekg(0 + sizeof(Metadata), ios::beg);
            changing.read(reinterpret_cast<char *>(&activating), sizeof(FileEntry));
            int count = 0;
            while (count <= entries)
            {
                if (strcmp(activating.fileName, "root") == 0)
                {
                    activating.current = true;
                    int current_pos = changing.tellp();
                    changing.seekp(current_pos - sizeof(FileEntry), ios::beg);
                    changing.write(reinterpret_cast<char *>(&activating), sizeof(FileEntry));
                    changing.close();
                    cout << "Directory changed\n";
                    return;
                }
                changing.read(reinterpret_cast<char *>(&activating), sizeof(FileEntry));
                count++;
            }

            changing.close();
            return;
        }
        changing.read(reinterpret_cast<char *>(&active), sizeof(FileEntry));
        cont++;
    }
    changing.close();
    cout << "Failed to change directory \n";
}

void FileSystem::modifyFather(FileEntry father, int son)
{
    fstream modify(file, ios::in | ios::out | ios::binary);
    if (!modify)
    {
        cerr << "File Error" << endl;
        modify.close();
        return;
    }

    FileEntry modfather;
    modify.seekg(0 + sizeof(Metadata));
    modify.read(reinterpret_cast<char *>(&modfather), sizeof(FileEntry));
    if (father.code == modfather.code)
    {
        if (father.firstson == -1)
        {
            father.firstson = son;
            father.lastson = son;
            int currentpos = modify.tellp();
            modify.seekp(currentpos - sizeof(FileEntry));
            modify.write(reinterpret_cast<char *>(&father), sizeof(FileEntry));
            modify.close();
        }
    }
}

void FileSystem::modifyBrother(FileEntry bro, int bronum)
{
    fstream modify(file, ios::in | ios::out | ios::binary);
    if (!modify)
    {
        cerr << "File Error" << endl;
        modify.close();
        return;
    }

    FileEntry brother;
    int cont = 0;
    modify.seekg(0 + sizeof(Metadata));
    modify.read(reinterpret_cast<char *>(&brother), sizeof(FileEntry));
    while (cont < entries)
    {
        if (bro.father == brother.father)
        {
            if (bro.rightbrother == -1)
            {
                bro.rightbrother = bronum;
                int currentpos = modify.tellp();
                modify.seekp(currentpos - sizeof(FileEntry));
                modify.write(reinterpret_cast<char *>(&bro), sizeof(FileEntry));
                modify.close();
                return;
            }
        }
        modify.read(reinterpret_cast<char *>(&brother), sizeof(FileEntry));
        cont++;
    }
}

void FileSystem::rm(char remove[30])
{
    fstream r(file, ios::in | ios::out | ios::binary);
    if (!remove || find(remove) == false || strcmp(remove, "root") == 0)
    {
        cerr << "Error\n";
        r.close();
        return;
    }

    FileEntry reading;
    int cont = 0;

    r.seekg(0 + sizeof(Metadata));
    r.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
    while (cont < entries)
    {
        if (strcmp(reading.fileName, remove) == 0 && reading.firstson == -1 && reading.used == true)
        {
            reading.used = false;
            int currentpos = r.tellp();
            r.seekp(currentpos - sizeof(FileEntry));
            r.write(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
            r.close();
            downMeta();
        }

        r.read(reinterpret_cast<char *>(&reading), sizeof(FileEntry));
        cont++;
    }
}