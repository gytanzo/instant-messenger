#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "serialize.h"
char UBIT[NAME_SIZE] = "bdm23";

/* Pack the user input provided in input into the appropriate message
 * type in the space provided by packed.  You can assume that input is a
 * NUL-terminated string, and that packed is a buffer of size
 * PACKET_SIZE.
 *
 * Returns the message type for valid input, or -1 for invalid input.
 */
int pack(void *packed, char *input) {
    return -1;
}

/* Create a refresh packet for the given message ID.  You can assume
 * that packed is a buffer of size PACKET_SIZE.
 *
 * You should start by implementing this method!
 *
 * Returns the message type.
 */
int pack_refresh(void *packed, int message_id) {
    char packageType = 0;
    int ubitIndex = 0, packedIndex = 1;

    *(int *)(packed) = packageType;

    while (ubitIndex != NAME_SIZE - 1){
        *(char *)(packed + packedIndex) = UBIT[ubitIndex];
        packedIndex++;
        ubitIndex++;
    }

    *(int *)(packed + packedIndex) = message_id;
    
    return 0;
}
