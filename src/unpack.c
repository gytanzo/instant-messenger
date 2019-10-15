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
int unpack(char *unpacked, void *packed) {
    return 0;
}

/* Unpack the given packed message into the given statistics structure.
 * You can assume that packed points to a message buffer large enough to
 * hold the statistics message, but you cannot assume that it is
 * otherwise valid.  You can assume that statistics points to a
 * statistics structure.
 *
 * Returns the message type that was unpacked, or -1 if it is invalid.
 */
int unpack_statistics(struct statistics *statistics, void *packed) {
    return 0;
}
