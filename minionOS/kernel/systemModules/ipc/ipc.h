#ifndef KERNEL_SYSTEMMODULES_IPC_IPC_H_
#define KERNEL_SYSTEMMODULES_IPC_IPC_H_

#include <inttypes.h>

#define MAX_MESSAGE_LENGTH  (100)

#define SUCCESS             (0)
#define QUEUE_FULL          (-1)
#define MESSAGE_TOO_LONG    (-2)

typedef struct {
    char message[MAX_MESSAGE_LENGTH];
    uint8_t sender;
} Message_t;

int ipc_message(uint8_t receiver, const char* messageString);
const Message_t* ipc_receive();


#endif /* KERNEL_SYSTEMMODULES_IPC_IPC_H_ */
