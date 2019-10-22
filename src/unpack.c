#include <stdlib.h>
#include <string.h>

#include "serialize.h"

/* Unpack the given message into the buffer unpacked.  You can assume
 * that packed points to a message buffer large enough to hold the
 * message described therein, but you cannot assume that the message is
 * otherwise valid.  You can assume that unpacked points to a character
 * buffer large enough to store the unpacked message, if it is a valid
 * message.
 *
 * Returns the message type that was unpacked, or -1 if it is invalid.
 */

#define intPad sizeof(int) /* Length of a int in bytes */
#define sizeTPad sizeof(size_t) /* Length of a size_t in bytes */
#define longPad sizeof(long) /* Length of a long in bytes */
#define charPad sizeof(char) /* Length of a char in bytes */

int indexUpdate(int type, int labelvar, void *packed){
    int packedIndex = 20; /* The first non-constant index will ALWAYS start at 20. */
    
    if (type == STATUS){ /* Status message */
        packedIndex += sizeTPad; /* Accounting for the "status length" part of the pointer */
        packedIndex += sizeTPad; /* Accounting for the "0" part of the pointer */
        return packedIndex; /* The status starts here. */
    }
    
    else if (type == MESSAGE){ /* Normal message. */
        packedIndex += sizeTPad; /* Accounting for the "message length" part of the pointer */
        packedIndex += sizeTPad; /* Accounting for the "0" part of the pointer */
        return packedIndex; /* The message starts here. */
    }
	
    else if (type == LABELED && labelvar == 1){ /* Label message where I return the target index. */
        packedIndex += sizeTPad; /* Accounting for the "message length" part of the pointer */
        packedIndex += sizeTPad; /* Accounting for the "target length" part of the pointer */
        packedIndex += sizeTPad; /* Accounting for the "0" part of the pointer */
        
        int messageLength = *(size_t*)(packed + 20); /* For labeled messages, the length of the message is here. */
        
        packedIndex += (charPad * messageLength); /* Accounting for the message part of the pointer */
        return packedIndex;
    }
    
    else if (type == LABELED && labelvar == 2){ /* Label message where I return the message index. */
        packedIndex += sizeTPad; /* Accounting for the "message length" part of the pointer */
        packedIndex += sizeTPad; /* Accounting for the "target length" part of the pointer */
        packedIndex += sizeTPad; /* Accounting for the "0" part of the pointer */
        return packedIndex;
    }

    return -1; /* Error: Likely an invalid type. */
}

int unpack(char *unpacked, void *packed) {
    int type = *(int *)packed, packedIndex;
    
    char username[NAME_SIZE];
    int index = 0;
    for (int userI = 4; userI != 20; userI++){
        char character = *(char *)(packed + userI);
        *(char *)(username + index) = character;
        index++;
    }
    int usernameLength = strlen(username);
    
    if (type == STATUS){
        packedIndex = indexUpdate(type, 1, packed);
        
        int statusLength = *(size_t*)(packed + 20); /* For statuses, the length of the status is located here */
        
        for (int i = 0; i != usernameLength; i++){
            char character = *(char *)(username + i);
            *(char *)unpacked = character;
            unpacked = unpacked + charPad;
        }

        *(char *)unpacked = ' ';
        unpacked = unpacked + charPad;
        
        for (int i = 0; i != statusLength; i++){
            char character = *(char *)(packed + packedIndex);
            *(char *)unpacked = character;
            unpacked = unpacked + charPad;
            packedIndex++;
        }

        *(char *)unpacked = '\0';
        
        return type;
    }

    else if (type == MESSAGE){
        packedIndex = indexUpdate(type, 1, packed);
        char *colon = ": ";
        
        int messageLength = *(size_t*)(packed + 20); /* For messages, the length of the message is located here. */
        int colonLength = strlen(colon);

        for (int i = 0; i != usernameLength; i++){
            char character = *(char *)(username + i);
            *(char *)unpacked = character;
            unpacked = unpacked + charPad;
        }
        
        for (int i = 0; i != colonLength; i++){
            char character = *(char *)(colon + i);
            *(char *)unpacked = character;
            unpacked = unpacked + charPad;
        }
        
        for (int i = 0; i != messageLength; i++){
            char character = *(char *)(packed + packedIndex);
            *(char *)unpacked = character;
            unpacked = unpacked + charPad;
            packedIndex++;
        }

        *(char *)unpacked = '\0';

        return type;
    }

    else if (type == LABELED){
        int packedIndexTarget = indexUpdate(type, 1, packed);
        int packedIndexMessage = indexUpdate(type, 2, packed);
        char *colon = ": ";
        
        int messageLength = *(size_t*)(packed + 20); /* For label messages, the length of the message is located here. */
        int targetLength = *(size_t*)(packed + 28); /* For label messages, the length of the target is located here. */
        int colonLength = strlen(colon);

        for (int i = 0; i != usernameLength; i++){
            char character = *(char *)(username + i);
            *(char *)unpacked = character;
            unpacked = unpacked + charPad;
        }
        
        for (int i = 0; i != colonLength; i++){
            char character = *(char *)(colon + i);
            *(char *)unpacked = character;
            unpacked = unpacked + charPad;
        }
        
        *(char *)unpacked = '@';
        unpacked = unpacked + charPad;
        
        for (int i = 0; i != targetLength; i++){
            char character = *(char *)(packed + packedIndexTarget);
            *(char *)unpacked = character;
            unpacked = unpacked + charPad;
            packedIndexTarget++;
        }
        
        *(char *)unpacked = ' ';
        unpacked = unpacked + charPad;
        
        for (int i = 0; i != messageLength; i++){
            char character = *(char *)(packed + packedIndexMessage);
            *(char *)unpacked = character;
            unpacked = unpacked + charPad;
            packedIndexMessage++;
        }
        
        *(char *)unpacked = '\0';
        
        return type;
    }
    
    return -1; /* Invalid message. */
}

/* Unpack the given pacd message into the given statistics structure.
 * You can assume that packprinted points to a message buffer large enough to
 * hold the statistics message, but you cannot assume that it is
 * otherwise valid.  You can assume that statistics points to a
 * statistics structure.
 *
 * Returns the message type that was unpacked, or -1 if it is invalid.
 */
int unpack_statistics(struct statistics *statistics, void *packed) {
    int type = *(int*)packed;

    if (type != STATISTICS){
        return -1; /* Error, what are you even doing here? */
    }
    
    int activeUserIndex = 20; /* For a statistics package, the most active user is located here. */
    int activeCountIndex = 36; /* For a statistics package, the most active count is located here. */
    int invalidCount = 40; /* For a statistics package, the invalid count is located here. */
    int refreshCount = 48; /* For a statistics package, the refresh count is located here. */
    int messageCount = 56; /* For a statistics package, the message count is located here. */

    int messages_count;
    int most_active_count;
    long invalid_count;
    long refresh_count;

    char username[NAME_SIZE];
    int index = 0;
    for (int userI = 4; userI != 20; userI++){
        char character = *(char *)(packed + userI);
        *(char *)(username + index) = character;
        index++;
    }

    int i = 0;
    while (i != 16){
        statistics -> sender[i] = username[i];
        i++;
    }

    int ma = 0;
    for (int i = activeUserIndex; i != activeCountIndex; i++){
        char character = *(char *)(packed + i);
        statistics -> most_active[ma] = character;
        ma++;
    }

    messages_count = *(int *)(packed + messageCount);
    statistics -> messages_count = messages_count;

    most_active_count = *(int *)(packed + activeCountIndex);
    statistics -> most_active_count = most_active_count;

    invalid_count = *(long *)(packed + invalidCount);
    statistics -> invalid_count = invalid_count;

    refresh_count = *(long *)(packed + refreshCount);
    statistics -> refresh_count = refresh_count;

    return type;
}
