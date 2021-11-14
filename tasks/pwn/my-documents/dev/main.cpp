#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>

void menu(void);
void setup(void);
int OpenFile(void);
int ReadFile(void);
int CloseFile(void);
int CheckFilename(const std::string&);
int Feedback(void);

bool FileIsRead;
std::ifstream g_FileHndl;

int main()
{
    setup();
    size_t iterations = 0;

    while (true)
    {
        iterations++;

        if (iterations > 10)
        {
            std::cout << "{-} Too many actions!";
            exit(0);
        }

        menu();
        char option;

        std::cin >> option;

        if (option <= '0' || option > '4' )
        {
            std::cout << "{-} Invalid choice!" << std::endl;
            continue;
        }

        switch (option)
        {
            case '1':
                OpenFile();
                break;
            case '2':
                ReadFile();
                break;
            case '3':
                CloseFile();
                break;
            case '4':
                Feedback();
                return 0;
            default:
                std::cout << "{-} Invalid choice!" << std::endl;
                break;
        }
    }
    return 0;
};

void setup(void)
{
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);
};

void menu(void)
{
    std::cout << "==++== My documents ==++==" << std::endl;
    std::cout << "1. Open file" << std::endl;
    std::cout << "2. Read file" << std::endl;
    std::cout << "3. Close file" << std::endl;
    std::cout << "4. Exit" << std::endl;
    std::cout << "> ";
};

int CheckFilename(const std::string& Fname)
{
    size_t FilenameSize = Fname.size();
    size_t CntOfPrintable = std::count_if(Fname.begin(), Fname.end(), 
        [](unsigned char c) {return std::isprint(c); }
    );

    if (CntOfPrintable != FilenameSize)
    {
        return false;
    }

    for (auto c : Fname)
    {
        if (c == '\\' || c == '/')
        {
            return false;
        }
    }

    return true;
};

int OpenFile(void)
{
    if (g_FileHndl.is_open())
    {
        std::cout << "{-} Some file already opened!" << std::endl;
        return false;
    }

    std::string Filename;
    std::cout << "{?} Enter file name: ";
    std::cin >> Filename;

    if (!CheckFilename(Filename))
    {
        std::cout << "{-} Incorrect symbols in file name!" << std::endl;
        return false;
    }

    g_FileHndl.open("/tmp/" + Filename);
    std::cout << "{+} File <" << Filename << "> is opened!" << std::endl;
    FileIsRead = false;
    return true;
};

int ReadFile(void)
{
    if (!g_FileHndl.is_open())
    {
        std::cout << "{-} File not opened!" << std::endl;
        return false;
    }

    if (FileIsRead)
    {
        std::cout << "{-} The file has already been read!" << std::endl;
        return 0;
    }

    std::cout << "{?} Enter size to read: ";
    size_t ReadSize;
    std::cin >> ReadSize;

    if (ReadSize > 16 * 1024)
    {
        std::cout << "{-} Size of read is too big!" << std::endl;
        return false;
    }

    char* ReadBuffer = new char [ReadSize];

    g_FileHndl.getline(ReadBuffer, ReadSize);
	std::cout << "===+++=== File data ===+++===" << std::endl;
    std::cout << ReadBuffer << std::endl;

    FileIsRead = true;
    delete[] ReadBuffer;
    return true;
};

int CloseFile(void)
{ 
    if (!g_FileHndl.is_open())
    {
        std::cout << "{-} File not opened!" << std::endl;
        return false;
    }

    g_FileHndl.close();
    FileIsRead = false;

    std::cout << "{+} File is closed!" << std::endl;
    return true;
};

int Feedback(void)
{
    std::cout << "{?} Please leave us a feedback!" << std::endl;
    std::cout << "{?} Y\\N: ";

    std::string option;
    std::cin >> option;

    if (option[0] == 'Y')
    {
        std::cout << "{?} Enter feedback size: ";
        size_t FeedbackSize;
        std::cin >> FeedbackSize;

        char* Buffer = new char[FeedbackSize];
        std::cout << "{?} Enter feedback: ";
        read(0, Buffer, FeedbackSize);

        std::cout << "{?} Thank you!\nYour feedback: ";
        std::cout << Buffer; 
        std::cout << "\nWill be recorded in our feedback book!" << std::endl;

        delete[] Buffer;
        exit(0);
    }
    else
    {
        std::cout << "{!} Ok, goodbye!" << std::endl;
        exit(0);
    }

    return 0;
};