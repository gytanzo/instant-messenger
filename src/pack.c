#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "serialize.h"
#define intPad sizeof(int) /* Length of a int in bytes */
#define sizeTPad sizeof(size_t) /* Length of a size_t in bytes */
#define longPad sizeof(long) /* Length of a long in bytes */

char UBIT[NAME_SIZE] = "bdm23";
char mostActive[NAME_SIZE]; /* A placeholder array for the most active user */
int labelIndex = 0;

int initalize(void *packed, int type){ /* Sets the first 19 elements of packed */
    int ubitIndex = 0; /* Keeps track of where in the UBIT array I am */
    int packedIndex = 0; /* Keeps track where in packed I am*/

    *(int *)(packed + packedIndex) = type;
    packedIndex = intPad;

    while (ubitIndex != NAME_SIZE - 1){ 
        *(char *)(packed + packedIndex) = UBIT[ubitIndex];
        packedIndex++;
        ubitIndex++;
    }

    if (packedIndex != 19){
        printf("%s%d%s\n", "Error: packedIndex has a value of ", packedIndex, " when it should be 19.");
        return 0;
    }

    else {
        return packedIndex + 1; /* returns a value pointing to the next index, which SHOULD be 19 */
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
    int length = strlen(input);
    
    if (!validity(input)){
        return -1;
    }

    if (input[0] == '/' && input[1] == 'm' && input[2] == 'e'){
        int check = -1;
        
        if (input[3] != ' '){
            puts("Error: Invalid character following the '/me'.");
            return -1;
        }
        
        for (int i = 3; i < length - 1; i++){ /* 3 represents the length of a "/me" */
            if (input[i] != ' '){
                check = 0;
            }
        }
        
        if (check != 0){
            puts("Error: Status message is completely composed of ASCII space characters.");
            return -1;
        }
        
        else {
            return 1; /* This is a status message. */
        }
    }

    else if (input[0] == '@'){
        if (input[1] == ' '){
            puts("Error: You didn't provide a username to tag.");
            return -1;
        }
        
        int index = -1;
        for (int i = 1; i <= 17; i++){
            if (input[i] == ' '){
                index = i;
                i = 17;
            }
        }


		labelIndex = index - 1; /* labelIndex returns the index at which the target ENDS (the 'b' in elb) */
		
        if (index == -1){
            puts("Error: The username you provided is too long.");
            return -1;
        }
        
        return 3; /* This is a labeled message. */
    }

    else if (input[0] == '/' && input[1] == 's' && input[2] == 't' && input[3] == 'a' && input[4] == 't' && input[5] == 's'){
        int length = strlen(input);
        if (length > 7){
            puts("Error: '/stats' call too long.");
            return -1;
        }
        
        if (input[6] != ' '){
            puts("Error: Invalid character following the '/stats'.");
            return -1;
        }
        else {
            return 4; /* This is a statistics message. */
        }
    }

    else {
        return 2; /* This is a plain message. */
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
    int type = inputCheck(input);
    size_t length = strlen(input);
    int packedLoc = initalize(packed, type); /* Initializes the package (adds type and UBIT) */
    
    if (type == -1){
        return -1; /* Invalid input (I would have already printed the error code, so I won't add one here). */
    }

    else if (type == 1){ /* Status message */
        int statusHead = 3; /* As stated previously, 3 represents the length of "/me" */
        while (input[statusHead] == ' '){
            statusHead++;
        }
    
        length -= statusHead;
        *(size_t *)(packed + packedLoc) = length;
        packedLoc += sizeTPad;

        *(size_t *)(packed + packedLoc) = 0;
        packedLoc += sizeTPad;

        for (int i = statusHead; i <= strlen(input); i++){
            *(char *)(packed + packedLoc) = input[i];
            packedLoc++;
        }
        
        return type;
    }

    else if (type == 2){ /* Normal message */
        *(size_t *)(packed + packedLoc) = length;
        packedLoc += sizeTPad;

        *(size_t *)(packed + packedLoc) = 0;
        packedLoc += sizeTPad;

        for (int i = 0; i <= strlen(input); i++){
            *(char *)(packed + packedLoc) = input[i];
            packedLoc++;
        }
        
        return type;
    }
	
	else if (type == 3){ /* Labeled message */
        *(size_t *)(packed + packedLoc) = length - 1 - (labelIndex + 1); /* The '@' char adds an extra index */
        packedLoc += sizeTPad;
        
        *(size_t *)(packed + packedLoc) = labelIndex;
        packedLoc += sizeTPad;
        
        *(size_t *)(packed + packedLoc) = 0;
        packedLoc += sizeTPad;
        
        int firstChar = labelIndex + 2; /* First index of the message */
        for (int i = firstChar; i <= strlen(input) - 1; i++){
            *(char *)(packed + packedLoc) = input[i];
            packedLoc++;
        }
        
        int firstTag = 1; /* First index of the target name */
        for (int i = firstTag; i <= labelIndex + 1; i++){
            *(char *)(packed + packedLoc) = input[i];
            packedLoc++;
        }
        
        return type;
    }
	
	else if (type == 4){ /* Stats message */
        int activeIndex = 0;
        while (activeIndex != NAME_SIZE - 1){ 
            *(char *)(packed + packedLoc) = mostActive[activeIndex];
            packedLoc++;
            activeIndex++;
        }
        
        *(int *)(packed + packedLoc) = 0;
        packedLoc += intPad;
        
        *(long *)(packed + packedLoc) = 0;
        packedLoc += longPad;
        
        *(long *)(packed + packedLoc) = 0;
        packedLoc += longPad;
        
        *(int *)(packed + packedLoc) = 0;
        packedLoc += intPad;
        
        return type;
    }

    puts("How did you get here? This is quite impressive.");
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
    
    return packageType;
}
