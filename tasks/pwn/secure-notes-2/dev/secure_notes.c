#include "secure_notes.h"

static noinline long ioctlHandler(struct file *file, unsigned int cmd, unsigned long arg){
    long result = -1;
    UserRequest* uReq = (UserRequest*) kmalloc(sizeof(UserRequest), GFP_KERNEL);

    if (copy_from_user((void *)uReq, (void *)arg, sizeof(UserRequest))){
        printk("{-} Some error in copy_from_user!");
        return result;
    }

    switch(cmd){
        case CREATE:
            result = create(uReq);
            break;
        case GET:
            result = get(uReq);
            break;
        case UPDATE:
            result = update(uReq);
            break;
        case DELETE:
            result = delete(uReq);
            break;
        case STATUS:
            result = status(uReq);
            break;
        default:
            result = NO_SUCH_OPTION;
            break;
    }

    kfree(uReq);
    return result;
};

static noinline long create(UserRequest* uReq) {
    Note* newNote;
    // check free space in list
    if (!check_free_space()) {
        return DONT_HAVE_FREE_SLOTS;
    }

    // check value of requested idx
    if (uReq->idx < 0 || uReq->idx >= NOTE_LIST_SIZE) {
        return INVALID_IDX_VALUE;
    }

    // check if requested idx is free
    if (!check_idx_is_free(uReq->idx)) {
        return SLOT_IS_USED;
    }

    // check input data size
    if (uReq->inSize > NOTE_DATA_MAX_SIZE) {
        return SIZE_IS_TOO_BIG;
    }

    // create Note object
    newNote = (Note*) kmalloc(sizeof(Note), GFP_KERNEL);
    newNote->isUsed = 1;
    newNote->idx = uReq->idx;

    // create note data chunk and copy data from user memory
    newNote->data = (char*) kmalloc(uReq->inSize, GFP_KERNEL);
    copy_from_user(newNote->data, uReq->inData, uReq->inSize);

    // generate random access token
    get_random_bytes(&newNote->access_token, sizeof(newNote->access_token));
    copy_to_user(uReq->token, &newNote->access_token, sizeof(uint64_t));
    //*uReq->token = newNote->access_token;

    // add new note to list
    list[newNote->idx] = newNote;
    LastAdded = newNote->idx;
    update_list_cnt();

    return NOTE_IS_CREATED;
};

static noinline long get(UserRequest* uReq) {
    uint64_t token;
    // check value of requested idx
    if (uReq->idx < 0 || uReq->idx >= NOTE_LIST_SIZE) {
        return INVALID_IDX_VALUE;
    }

    // check if requested idx is not free
    if (check_idx_is_free(uReq->idx)) {
        return SLOT_IS_NOT_USED;
    }

    // check if idx is the same
    if (list[uReq->idx]->idx != uReq->idx) {
        return IDXS_IS_NOT_EQUAL;
    }

    // check access_token
    copy_from_user(&token, uReq->token, sizeof(uint64_t));
    if (token != list[uReq->idx]->access_token) {
        return ACCESS_DENIED;
    }

    // copy data from note to user buffer
    copy_to_user(uReq->outData, list[uReq->idx]->data, uReq->outSize); // here is memory leak
    return NOTE_GET;
};

static noinline long update(UserRequest* uReq) {
    uint64_t token;
    // check value of requested idx
    if (uReq->idx < 0 || uReq->idx >= NOTE_LIST_SIZE) {
        return INVALID_IDX_VALUE;
    }

    // check if requested idx is free
    if (check_idx_is_free(uReq->idx)) {
        return SLOT_IS_NOT_USED;
    }

    // check input data size
    if (uReq->inSize > NOTE_DATA_MAX_SIZE) {
        return SIZE_IS_TOO_BIG;
    }

    // check if idxs is the same
    if (list[uReq->idx]->idx != uReq->idx) {
        return IDXS_IS_NOT_EQUAL;
    }

    // check access token
    copy_from_user(&token, uReq->token, sizeof(uint64_t));
    if (token != list[uReq->idx]->access_token) {
        return ACCESS_DENIED;
    }

    // copy data from user to note data
    copy_from_user(list[uReq->idx]->data, uReq->inData, uReq->inSize); // here is heap-overflow
    return NOTE_IS_UPDATED;
};

static noinline long delete(UserRequest* uReq) {
    uint64_t token;
    // check value of requested idx
    if (uReq->idx < 0 || uReq->idx >= NOTE_LIST_SIZE) {
        return INVALID_IDX_VALUE;
    }

    // check if requested idx is free
    if (check_idx_is_free(uReq->idx)) {
        return SLOT_IS_NOT_USED;
    }

    // check if idx is the same
    if (list[uReq->idx]->idx != uReq->idx) {
        return IDXS_IS_NOT_EQUAL;
    }

    // check access token
    copy_from_user(&token, uReq->token, sizeof(uint64_t));
    if (token != list[uReq->idx]->access_token) {
        return ACCESS_DENIED;
    }

    // free and null data pointer
    kfree(list[uReq->idx]->data);
    list[uReq->idx]->data = NULL;
    
    // rewrite isUsed and access_token
    list[uReq->idx]->isUsed = 0;
    list[uReq->idx]->access_token = 0;
    
    // free and null note pointer
    kfree(list[uReq->idx]);
    list[uReq->idx] = NULL;

    update_list_cnt();

    return NOTE_IS_DELETED;
};

static noinline long status(UserRequest* uReq) {
    char* statusBuffer = (char*) kmalloc(STATUS_BUFFER_SIZE, GFP_KERNEL);
    update_list_cnt();
    
    // add info about max list size
    sprintf(statusBuffer, 
        "List max size: %d\nList current size: %d\nLast added note idx: %d\n",
        NOTE_LIST_SIZE, ListCnt, LastAdded);

    if (uReq->outSize < strlen(statusBuffer)) {
        return OUT_SIZE_IS_TOO_SMALL;
    }

    copy_to_user(uReq->outData, statusBuffer, strlen(statusBuffer));
    return NOTE_STATUS;
};

static noinline uint8_t check_free_space(void) {
    update_list_cnt();
    if (ListCnt == NOTE_LIST_SIZE) {
        return 0;
    } else {
        return 1;
    }
};

static noinline uint32_t update_list_cnt(void) {
    int i;
    ListCnt = 0;
    for (i = 0; i < NOTE_LIST_SIZE; i++) {
        if (list[i] != NULL) {
            if (list[i]->isUsed != 0)
                ListCnt += 1;
        }
    }
    return ListCnt;
};

static noinline uint8_t check_idx_is_free(uint32_t idx) {
    if (list[idx] != NULL) {
        if (list[idx]->isUsed)
            return 0;
    } 
    return 1;
};

static int __init init_dev(void){
    int i = 0;
    for (i; i < NOTE_LIST_SIZE; i++) {
        list[i] = NULL;
    }
    
    reg = misc_register(&secure_notes_dev);
    if (reg < 0){
        printk("[-] Failed to register secure_notes!");
    }
    return 0;
};

static void __exit exit_dev(void){
    int i;
    for (i = 0; i < NOTE_LIST_SIZE; i += 1) {
        if (!check_idx_is_free(i)) {
            kfree(list[i]->data);
            kfree(list[i]);
        }
    }
    misc_deregister(&secure_notes_dev);
}

module_init(init_dev);
module_exit(exit_dev);