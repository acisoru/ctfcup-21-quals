#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#define MAX_FLOOR_IDX 15
#define FLOOR_LIST_SIZE 16
#define MAX_NAME_SIZE 64

char* menu = "+++==+++ Mnogoetazhka v2+++==+++\n"\
"1. Make floor\n"\
"2. Edit floor name\n"\
"3. Delete floor\n"\
"4. View floor\n"\
"5. Exit\n"\
"> ";

void setup(void);
int make_floor(void);
int edit_floor(void);
int delete_floor(void);
int view_floor(void);
int FloorListNotFull(void);

typedef struct floor {
    uint64_t Number;
    char* Name;
} floor;

floor* FloorList[16];
int ListIdx;

int main()
{
    setup();

    while (1)
    {
        printf(menu);
        int option;
        scanf("%d", &option);

        if (option < 1 || option > 4)
            break;
        
        switch (option)
        {
            case 1:
                make_floor();
                break;
            case 2:
                edit_floor();
                break;
            case 3:
                delete_floor();
                break;
            case 4:
                view_floor();
                break;
            case 5:
                return 0;
        }
    }

    return 0;
}

void setup(void)
{
    ListIdx = 0; 
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);
};

int make_floor(void)
{
    if (!FloorListNotFull())
    {
        puts("{-} Floor list is full!");
        return 0;
    }

    floor* newFloor = (floor*) malloc(sizeof(floor));

    printf("{?} Enter floor number: ");
    scanf("%d", &newFloor->Number);
    printf("{?} Enter floor name size: ");
    int FloorNameSize = 0;
    scanf("%d", &FloorNameSize);

    if (FloorNameSize < 0 || FloorNameSize > MAX_NAME_SIZE)
    {
        puts("{-} Invalid floor name size!");
        return 0;
    }
    
    newFloor->Name = (char*) malloc(FloorNameSize);
    printf("{?} Enter floor name: ");
    int nbytes = read(0, newFloor->Name, FloorNameSize);

    FloorList[ListIdx++] = newFloor;
};

int edit_floor(void)
{
    printf("{?} Enter floor idx: ");
    int idx = 0;

    scanf("%d", &idx);

    if (idx < 0 || idx > MAX_FLOOR_IDX)
    {
        puts("{-} Incorrect idx!");
        return 0;
    }

    if (FloorList[idx] == NULL)
    {
        puts("{-} No such floor!");
        return 0;
    }

    printf("{?} Enter new floor size: ");
    int NewSize = 0;
    scanf("%d", &NewSize);

    if (NewSize < 0 || NewSize > MAX_NAME_SIZE)
    {
        puts("{-} Error size!");
        return 0;
    }

    FloorList[idx]->Name = realloc(FloorList[idx]->Name, NewSize);
    printf("{?} Enter new name: ");
    int nbytes = read(0, FloorList[idx]->Name, NewSize);
};

int delete_floor(void)
{
    printf("{?} Enter floor idx: ");
    int idx = 0;

    scanf("%d", &idx);

    if (idx < 0 || idx > MAX_FLOOR_IDX)
    {
        puts("{-} Incorrect idx!");
        return 0;
    }

    if (FloorList[idx] == NULL)
    {
        puts("{-} No such floor!");
        return 0;
    }

    free(FloorList[idx]->Name);
    free(FloorList[idx]);
};

int view_floor(void)
{
    printf("{?} Enter floor idx: ");
    int idx = 0;

    scanf("%d", &idx);

    if (idx < 0 || idx > MAX_FLOOR_IDX)
    {
        puts("{-} Incorrect idx!");
        return 0;
    }

    if (FloorList[idx] == NULL)
    {
        puts("{-} No such floor!");
        return 0;
    }

    printf("Floor #%d\n", idx);
    printf("Name: %s\n", FloorList[idx]->Name);
    printf("Number: %lld\n", FloorList[idx]->Number);
};

int FloorListNotFull(void)
{
    return ListIdx == FLOOR_LIST_SIZE ? 0 : 1;
}