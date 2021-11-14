#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h> 
#include <linux/random.h>

MODULE_AUTHOR("the_lothric@localhost");                    
MODULE_DESCRIPTION("Secure notes");
MODULE_LICENSE("GPL");

#define NOTE_LIST_SIZE 32
#define NOTE_DATA_MAX_SIZE 0x200
#define STATUS_BUFFER_SIZE 0x300

#define CREATE 0x1001
#define GET    0x2002
#define UPDATE 0x3003
#define DELETE 0x4004
#define STATUS 0x5005

static enum Corrects {
    NOTE_IS_CREATED = 0x100,
    NOTE_IS_UPDATED,
    NOTE_IS_DELETED,
    NOTE_GET,
    NOTE_STATUS
};

static enum Errors {
    NO_SUCH_OPTION = 0x10,
    DONT_HAVE_FREE_SLOTS,
    SLOT_IS_USED,
    INVALID_IDX_VALUE,
    SIZE_IS_TOO_BIG, 
    SLOT_IS_NOT_USED,
    ACCESS_DENIED,
    IDXS_IS_NOT_EQUAL,
    OUT_SIZE_IS_TOO_SMALL
};

typedef struct{
    uint32_t idx;
    char* inData;
    uint32_t inSize;
    char* outData;
    uint32_t outSize;
    uint64_t* token;
} UserRequest;

typedef struct {
    uint32_t idx;
    char* data;
    uint64_t access_token;
    uint8_t isUsed;
} Note;

Note* list[NOTE_LIST_SIZE];
uint32_t ListCnt;
uint32_t LastAdded;
int reg;

static noinline long ioctlHandler(struct file *file, unsigned int cmd, unsigned long arg);
static struct file_operations secure_notes_fops = {.unlocked_ioctl = ioctlHandler}; 

static noinline long create(UserRequest* uReq);
static noinline long get(UserRequest* uReq);
static noinline long update(UserRequest* uReq);
static noinline long delete(UserRequest* uReq);
static noinline long status(UserRequest* uReq);

// helpers
static noinline uint8_t check_free_space(void);
static noinline uint32_t update_list_cnt(void);
static noinline uint8_t check_idx_is_free(uint32_t);

struct miscdevice secure_notes_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "secure_notes",
    .fops = &secure_notes_fops,
};