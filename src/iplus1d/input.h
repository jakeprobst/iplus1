#ifndef _INPUT_H_
#define _INPUT_H_

typedef enum command_type_t  {
    CMD_SENTENCE,
    CMD_LINK,
} command_type_t;


typedef struct command_t {
    command_type_t type;
    int fd;
    union {
        struct {
            int id;
            char lang[4];
            char* sen;
        } sen;
        struct {
            int id;
            int tid;
        } link;
    };
    
} command_t;







typedef struct input_t {
    int epoll_fd;
    int listen_fd;
    
    int (*callback)(command_t*, void*);
    void* callback_param;
    
} input_t;




int input_init(input_t*);
int input_destroy(input_t*);
int input_set_callback(input_t*, int (*func)(command_t*, void*), void*);
int input_loop(input_t*);


int command_destroy(command_t*);


























#endif /* _INPUT_H_ */
