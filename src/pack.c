#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "serialize.h"

char UBIT[NAME_SIZE] = "bdm23";

int initalize(void *packed, int type){ /* Sets the first 17 elements of packed */
    int ubitIndex = 0; /* Keeps track of where in the UBIT array I am */
    int packedIndex = 0; /* Keeps track where in packed I am*/

    *(int *)(packed + packedIndex) = type;
    packedIndex++;

    while (ubitIndex != NAME_SIZE - 1){ 
        *(char *)(packed + packedIndex) = UBIT[ubitIndex];
        packedIndex++;
        ubitIndex++;
    }

    if (packedIndex != 16){
        printf("%s%d%s\n", "Error: packedIndex has a value of", packedIndex, " when it should be 16.");
        return 0;
    }

    else {
        return packedIndex;
    }
}

bool validity(char *input){
    int length = strlen(input);

    if (length > 256){
        puts("Error: Message length exceeds MAX_MESSAGE_SIZE.");
        return false;
    }
    
    for (int i = 0; i <= length - 1; i++){
        if (input[i] != ' '){
            return true;
        }
    }
    
    puts("Error: Message is completely composed of ASCII space characters.");
    return false;
}

int inputCheck(char *input) {
    if (!validity(input)){
        return -1;
    }
    
    if (input[0] == '/' && input[1] == 'm' && input[2] == 'e'){
        return 1; /* This is a status message. */
    }

    else if (input[0] == '@'){
        if (input[1] == ' '){
            puts("Error: You didn't provide a username to tag.");
            return -1;
        }
        
        int index = -1;
        
        for (int i = 1; i < 17; i++){
            if (input[i] == ' '){
                index = i;
                i = 16;
            }
        }

        if (index == -1){
            puts("Error: The username you provided is too long.");
            return -1;
        }
        
        puts("This is a labeled message.");
        return 3;
    }

    puts("Error: Reached end of inputCheck.");
    return -1;
}

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
    char packageType = 0; /* The package type for this function will always be zero */

    int packedIndex = initalize(packed, packageType);

    *(int *)(packed + packedIndex) = message_id;
    
    return 0;
}
