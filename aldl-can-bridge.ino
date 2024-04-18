/*  MCP2517/8 send a can fd frame
    CAN FD Shield - https://www.longan-labs.cc/1030012.html
    CANBed FD - https://www.longan-labs.cc/1030009.html
    
    can-fd baud rate:
    CAN_125K_500K
    CAN_250K_500K
    CAN_250K_750K
    CAN_250K_1M
    CAN_250K_1M5
    CAN_250K_2M
    CAN_250K_3M
    CAN_250K_4M
    CAN_500K_1M
    CAN_500K_2M
    CAN_500K_3M
    CAN_500K_4M
    CAN_1000K_4M
*/

#include <SPI.h>
#include "mcp2518fd_can.h"
#include "linuxaldl.h"
#include "definitions/df.h"

#define SerialALDL Serial1
#define SerialDebug Serial

#define ALDL_MAX_MESSAGE_SIZE 128
#define CAN_MAX_FRAME_SIZE 8

#define CAN_BASE_PID 0xBAD

// pin for CAN-FD Shield
//const int SPI_CS_PIN = 9;
//const int CAN_INT_PIN = 2;

// pin for CANBed FD
const int SPI_CS_PIN = 17;
const int CAN_INT_PIN = 7;

mcp2518fd CAN(SPI_CS_PIN); // Set CS pin

unsigned char aldl_raw[ALDL_MAX_MESSAGE_SIZE] = {0};
unsigned char can_message[CAN_MAX_FRAME_SIZE] = {0};

aldl_definition_t* ALDLMask;


void SerialDebug_Init(){
    // init serial debug port (USB to computer)
    SerialDebug.begin(115200);
    while (!SerialDebug) {}
}

void ALDL_Init(){
    SerialALDL.begin(8192);
    while (!SerialALDL) {}
    SerialDebug.println("ALDL interface init ok!");
}

void ALDL_CalculateChecksum(unsigned char *aldl_raw, int len){
    // calculates the single-byte checksum, summing from the start of buffer
    // through len bytes minus 1. the checksum is calculated by adding each byte
    // together and ignoring overflow, then taking the two's complement and adding 1
	char acc = 0x00;
	unsigned int i;
	for (i=0; i<len-1; i++){
		acc+=buffer[i];
	}
	acc=0xFF-acc;
	acc+=0x01;
	return acc;
}

int ALDL_Read(unsigned char *aldl_raw, int len){
    if (len < ALDLMask->mode1_response_length) {
        SerialDebug.println("ALDL buffer too small!");
        return 0;
    }
    // get aldl mode 1 message
    // send MODE8 to silence the ECM
    SerialALDL.write(ALDLMask->mode8_request, ALDLMask->mode8_request_length);
    delay(10);
    // request MODE1 to get data
    SerialALDL.write(ALDLMask->mode1_request, ALDLMask->mode1_request_length);
    delay(10);
    // read data
    // set timeout
    SerialALDL.setTimeout(ALDLMask->mode1_response_length * 1);
    int len = SerialALDL.readBytes(aldl_raw, ALDLMask->mode1_response_length);
    // check data length
    if (len != ALDLMask->mode1_response_length) {
        SerialDebug.println("ALDL read data fail!");
        return 1;
    }
    // calculate checksum
    int checksum = ALDL_CalculateChecksum(aldl_raw, ALDLMask->mode1_response_length);
    if (checksum != aldl_raw[ALDLMask->mode1_response_length - 1]) {
        SerialDebug.println("ALDL checksum error!");
        return 1;
    }

}

void CAN_Init(){
    // init can bus : arbitration bitrate = 500k, data bitrate = 1M
    while (0 != CAN.begin(CAN_500K_4M)) {
        SerialDebug.println("CAN init fail, retry...");
        delay(100);
    }
    CAN.setMode(CAN_NORMAL_MODE);

    SerialDebug.println("CAN init ok!");

    byte mode = CAN.getMode();
    SerialDebug.print("CAN mode = ");
    SerialDebug.println(mode);

}

void CAN_Send(int base_pid, char *data, int len){
    // send data to CAN bus. split into 8 bytes per frame.
    int can_frame_pid = base_pid;
    for (int i = 0; i < len; i += CAN_MAX_FRAME_SIZE) {
        int frame_len = min(CAN_MAX_FRAME_SIZE, len - i);
        for (int j = 0; j < frame_len; j++) {
            can_message[j] = data[i + j];
        }
        CAN.sendMsgBuf(can_frame_pid, 0, frame_len, can_message);
        can_frame_pid++;
        delay(10);
    }
}

void init(){
    ALDLMask = &aldl_DF;
    SerialDebug_Init();
    ALDL_Init();
    CAN_Init();
}


void setup() {
    init();
}


void loop() 
{
    if (ALDL_MAX_MESSAGE_SIZE < ALDLMask->mode1_response_length) {
        SerialDebug.println("ERROR: ALDL buffer too small!");
        delay(1000);
        return;
    }
    int retval = ALDL_Read(aldl_raw, ALDL_MAX_MESSAGE_SIZE);
    if (retval != 0) {
        SerialDebug.println("ERROR: ALDL read fail!");
        delay(1000);
        return;
    }
    CAN_Send(CAN_BASE_PID, aldl_raw + ALDLMask->mode1_data_offset, ALDLMask->mode1_data_length);
    delay(100);
}
