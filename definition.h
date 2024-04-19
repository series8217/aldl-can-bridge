#ifndef _DEFINITION_H
#define _DEFINITION_H

#include <Arduino.h>

#define _ALDL_MESSAGE_MODE8 aldl_settings.definition->mode8_request, aldl_settings.definition->mode8_request_length
#define _ALDL_MESSAGE_MODE9 aldl_settings.definition->mode9_request, aldl_settings.definition->mode9_request_length

#define __MAX_REQUEST_SIZE 16 // maximum size (bytes) of a request message
							  // to send to the ECM

#define LINUXALDL_MODE1_END_DEF   \
	{                             \
		NULL, 0, 0, 0, 0, 0, NULL \
	}

typedef enum _ALDL_OP
{
	ALDL_OP_MULTIPLY = 0,
	ALDL_OP_DIVIDE = 1,
	ALDL_OP_SEPERATOR = 9
} ALDL_OP_t;

#define _DEF_SEP(label)                            \
	{                                              \
		label, 0, 0, ALDL_OP_SEPERATOR, 0, 0, NULL \
	}

// mapping of byte offsets to labels.
// labels must match the CAN DBC file and will be packed accordingly.
typedef struct
{
	const char *label;
	unsigned int aldl_byte_offset;
	unsigned int num_bytes;
	unsigned int can_pid_offset;
	unsigned int can_bits_offset;
} byte_map_t;

typedef struct _aldl_definition
{
	const char *name;
	byte mode1_request[__MAX_REQUEST_SIZE]; // the mode 1 request message, including the checksum
	unsigned int mode1_request_length;				 // the length of the mode 1 message including the checksum

	unsigned int mode1_response_length; // the total length of the response from the ecm

	unsigned int mode1_data_length; // the number of data bytes in the mode1 message response

	unsigned int mode1_data_offset; // the byte offest from the start of the mode1 message response
									// to the first byte of the data. e.g. if the data part of the
									// message is the 4th byte onward, this should be 3. (1+3 = 4)

	const byte_map_t *mode1_map; // pointer to start of table of byte_def_t structs.
						   // the last element must be LINUXALDL_MODE1_END_DEF

	byte mode8_request[__MAX_REQUEST_SIZE]; // the mode 8 (silence) request message, incl checksum
	unsigned int mode8_request_length;				 // the length of the mode 8 message incl checksum

	byte mode9_request[__MAX_REQUEST_SIZE]; // the mode 9 (un-silence) request message, incl checksum
	unsigned int mode9_request_length;				 // the length of the mode 9 message including the checksum

} aldl_definition_t;

#endif // _DEFINITION_H