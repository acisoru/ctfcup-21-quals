#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#define MAX_DATA_LIST_SIZE 16

// set patch  "\x48\x83\xc1\x08\x48\x8b\x21\xc3" - win rop gadget

FILE* rnd;
uint64_t money = 2048;
char UsedUntestedFunction = 0;
char* DataList[MAX_DATA_LIST_SIZE];

void DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

void setup()
{
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);

    rnd = fopen("/dev/urandom", "r");

    if (rnd == NULL)
    {
        puts("{-} Some server error! Tell admins!");
        exit(0);
    }
};

void menu(void)
{
    puts("---+++--- Random Data Shop ---+++---");
    puts("1. Buy data");
    puts("2. Sell data");
    puts("3. Change your data");
    puts("4. Exit");
    printf("> ");
};

int GetFreeIdx(void)
{
    for (int i = 0; i < MAX_DATA_LIST_SIZE; i++)
    {
        if (DataList[i] == NULL)
            return i;
    }
    return -1;
}

char* GetData(uint64_t size)
{
    char* buf = (char*) calloc(size, sizeof(char));
    fread(buf, sizeof(char), size, rnd);
    return buf;
};

int buy_data(void)
{
    int idx = GetFreeIdx();

    if (idx == -1)
    {
        puts("{-} Data list is full!");
        return 0;
    }

    puts("{!} Course: 1 data = 1 coin");
    printf("{!} You have %lu coins\n", money);
    printf("{?} Enter size of data which you want to buy: ");

    uint64_t UserInp;
    scanf("%lu", &UserInp);

    if (UserInp > money)
    {
        puts("{-} You don't have enough money!");
        return 0;
    }

    money -= UserInp;
    DataList[idx] = (char*) GetData(UserInp);
    puts("{+} Your data is: ");
    DumpHex(DataList[idx], UserInp);
};

int sell_data(void)
{
    uint64_t idx;
    printf("{?} Enter idx of data: ");
    scanf("%lu", &idx);

    if (idx < 0 || idx > MAX_DATA_LIST_SIZE)
    {
        puts("{-} Invalid idx!");
        return 0;
    }

    if (DataList[idx] == NULL)
    {
        puts("{-} No such data!");
        return 0;
    }

    size_t DataSize = malloc_usable_size(DataList[idx]);
    printf("{!} Your data cost is: %lu\n", DataSize);
    printf("{?} Do you want to sell this?[Y\\N]: ");

    char SellThis[8];
    read(0, SellThis, 8);

    if (SellThis[0] == 'N')
        return 0;
    
    money += DataSize;
  
    memset(DataList[idx], 0, DataSize);
    free(DataList[idx]);
    DataList[idx] = NULL;
};

int change_data(void)
{
    uint64_t idx;
    printf("{?} Enter idx of data: ");
    scanf("%lu", &idx);

    if (idx < 0 || idx > MAX_DATA_LIST_SIZE)
    {
        puts("{-} Invalid idx!");
        return 0;
    }

    if (DataList[idx] == NULL)
    {
        puts("{-} No such data!");
        return 0;
    }

    size_t DataSize = malloc_usable_size(DataList[idx]);

    printf("{?} Enter new data: ");
    read(0, DataList[idx], DataSize);

    puts("{+} Now your data is: ");
    DumpHex(DataList[idx], DataSize);
};

int view_data(void)
{
    puts("{!} This is untested function! Please don't use it in final build!");
    UsedUntestedFunction = 1;

    uint64_t idx;
    printf("{?} Enter idx of data: ");
    scanf("%lu", idx);

    if (idx < 0 || idx > 16)
    {
        puts("{-} Invalid idx!");
        return 0;
    }

    if (DataList[idx] == NULL)
    {
        puts("{-} No such data!");
        return 0;
    }

    size_t DataSize = malloc_usable_size(DataList[idx]);

    puts("{+} Now your data is: ");
    DumpHex(DataList[idx], DataSize);
};

int get_user_choice(void)
{
    char tmp[8];
    read(0, tmp, 8);
    return atoi(tmp);
};

int main()
{   
    setup();
    while (1 && !UsedUntestedFunction)
    {
        menu();
        int option = get_user_choice();

        switch (option)
        {
            case 1:
                buy_data();
                break;
            case 2:
                sell_data();
                break;
            case 3:
                change_data();
                break;
            case 0:
                view_data();
                break;
            case 4:
                return 0;
            default:
                fclose(rnd);
                exit(0);
        }
    }
    
    return 0;
};
