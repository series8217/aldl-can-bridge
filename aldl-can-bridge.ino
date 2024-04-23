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

      CAN Bus baudrare for CAN2.0 as below,

    CAN20_5KBPS
    CAN20_10KBPS
    CAN20_20KBPS
    CAN20_25KBPS
    CAN20_31K25BPS
    CAN20_33KBPS
    CAN20_40KBPS
    CAN20_50KBPS
    CAN20_80KBPS
    CAN20_83K3BPS
    CAN20_95KBPS
    CAN20_100KBPS
    CAN20_125KBPS
    CAN20_200KBPS
    CAN20_250KBPS
    CAN20_500KBPS
    CAN20_666KBPS
    CAN20_800KBPS
    CAN20_1000KBPS
*/

// #define BOARD_OBD2_GPS
#define BOARD_CANBED_FD_AVR
#define CAN_BAUD_RATE CAN20_500KBPS

// Debug Logging
//#define DEBUG_LOG_ALDL

#include <SPI.h>

#ifdef BOARD_CANBED_FD_AVR
#include "mcp2518fd_can.h"
// pin for CANBed FD
const int SPI_CS_PIN = 17;
const int CAN_INT_PIN = 7;
mcp2518fd CAN(SPI_CS_PIN); // Set CS pin
#endif

#ifdef BOARD_OBD2_GPS
#include "mcp_canbus.h"
MCP_CAN CAN(SPI_CS_PIN_MCP_CAN)
// SPI CS pin for the CAN bus chip
#define SPI_CS_PIN_MCP_CAN 9
    // this tells the CAN library which pin to use
    extern MCP_CAN CAN;
#endif

#include "df.h"

#define SerialALDL Serial1
#define SerialDebug Serial

#define ALDL_MAX_MESSAGE_SIZE 156
#define CAN_MAX_FRAME_SIZE 8

// base PID for the ALDL data
const unsigned long CAN_BASE_PID = 0x1BAD0;
#define USE_EXTENDED_PID 1

// PID for the status of this module, even if no ALDL
const unsigned long CAN_STATUS_PID = 0x2BAD0;

// pin for CAN-FD Shield
// const int SPI_CS_PIN = 9;
// const int CAN_INT_PIN = 2;

byte aldl_raw[ALDL_MAX_MESSAGE_SIZE] = {0};
byte can_message[CAN_MAX_FRAME_SIZE] = {0};

const aldl_definition_t *ALDLMask;

void SerialDebug_Init()
{
    /* Start the serial debug interface (USB to computer )*/
    SerialDebug.begin(115200);
}

void DebugPrintBuf(byte* buf, unsigned int len){
    // print the data as hex
    for (int i = 0; i < len; i++)
    {
        SerialDebug.print("0x");
        if (buf[i] < 16)
        {
            SerialDebug.print("0");
        }
        SerialDebug.print(buf[i], HEX);
        SerialDebug.print(",");
        if (i % 8 == 7)
        {
            SerialDebug.println();
        }
    }
    SerialDebug.println();
}

void ALDL_Init()
{
    /* Start the ALDL interface at its weird 8192 baud rate */
    SerialALDL.begin(8192);
    while (!SerialALDL)
    {
    }
    SerialDebug.println("ALDL UART init ok");
}

bool ALDL_VerifyChecksum(byte *buffer, int len)
{
    // calculates the single-byte checksum, summing from the start of buffer
    // through len bytes which includes the checksum. the result should be zero.
    byte acc = 0;
    for (unsigned int i = 0; i < len; i++)
    {
        acc += buffer[i];
    }
    return acc == 0;
}

void ALDL_SilenceBus()
{
    // become the bus master
    // send MODE8 to silence the ECM
    SerialALDL.write(ALDLMask->mode8_request, ALDLMask->mode8_request_length);
    SerialALDL.flush();
    delay(50);
}

int ALDL_ReadMode1(byte *result_buf, int buf_len)
{
    /* get a mode 1 message from the ECU. this is a long data stream with 
     * a plethora of useful content. */
    if (buf_len < ALDLMask->mode1_response_length)
    {
        SerialDebug.println("! ALDL buf too small");
        return 0;
    }
    // IMPROVE: can we do this in place on result_buf?
    byte scan_buf[ALDL_MAX_MESSAGE_SIZE];
    memset(scan_buf, 0xFF, sizeof(scan_buf));
    memset(result_buf, 0x00, sizeof(result_buf));

    // get aldl mode 1 message
    // request MODE1 to get data
    SerialALDL.write(ALDLMask->mode1_request, ALDLMask->mode1_request_length);
    SerialALDL.flush();
    // read data
    // first look for the header -- it has to be 0xF4 (header) 0x95 (XXX: why?) 0x01
    // we attempt to read at least as many bytes as the request plus a few more, that
    // way if we see the echo of our previous message we read past it.
    // IMPROVE: scanning function here read and scan until we see 0xF4 0x95 0x01.
    //          we also need to figure out why it's 0x95 so we can calculate that.
    //          it shoudl be the length + 85 decimal but that doesn't agree.
    // header for message from ECU
    const int START_SEQ_LENGTH = 3;
    byte mode1_resp_header[3];
    mode1_resp_header[0] = 0xF4;
    // XXX: why +1??? why 0x55?
    mode1_resp_header[1] = ALDLMask->mode1_data_length + 1 + 0x55;
    mode1_resp_header[2] = 0x01;

    // set timeout to just the time necessary to fill the buf if the bus is busy enough
    SerialALDL.setTimeout(ALDL_MAX_MESSAGE_SIZE * 0.125);
    int readlen = SerialALDL.readBytes(scan_buf,
                                       ALDL_MAX_MESSAGE_SIZE);
    // scan the received data for the start sequence
    bool match = 0;
    int i = 0;
    for (i = 0; i < readlen - START_SEQ_LENGTH; i++)
    {
        if (memcmp(scan_buf + i, mode1_resp_header, START_SEQ_LENGTH) == 0)
        {
            match = 1;
            break;
        }
    }
    // FIXME: this is not the correct length to copy. it should stop at end of message!!!
    unsigned int message_len = ALDL_MAX_MESSAGE_SIZE;
    // byte index 1 in the header is the size plus 0x52.
    unsigned int len_from_header = scan_buf[i + 1] - 0x52;
    if (len_from_header < ALDL_MAX_MESSAGE_SIZE)
    {
        message_len = len_from_header;
    }
    memcpy(result_buf, scan_buf + i, message_len);
    if (!match)
    {
        SerialDebug.println("! no header match");
    }
#ifdef DEBUG_LOG_ALDL
    if (!match){
      DebugPrintBuf(scan_buf, ALDL_MAX_MESSAGE_SIZE);
    } else {
      DebugPrintBuf(result_buf, message_len);
    }
#endif
    if (message_len < ALDLMask->mode1_response_length - 3)
    {
        SerialDebug.println("! ALDL message len wrong");
    }
    // verify data integrity
    if (!ALDL_VerifyChecksum(result_buf, message_len))
    {
        SerialDebug.println("! ALDL checksum error");
        return 1;
    }
#ifdef DEBUG_LOG_ALDL
    SerialDebug.println("message OK");
#endif
    return 0;
}


byte ALDL_CalculateChecksum(byte *buffer, int len)
{
    // calculates the single-byte checksum, summing from the start of buffer
    // through len bytes which doesn't include the checksum. returns the
    // value of the checksum to be added on that makes the result zero.
    byte acc = 0;
    for (unsigned int i = 0; i < len; i++)
    {
        acc += buffer[i];
    }
    return -acc;
}


int ALDL_ReadMode3(byte *result_buf, int len, uint16_t *request, int request_len)
{
    /* get a mode 3 message from the ECU. */
    if (len < request_len + 4)
    {
        SerialDebug.println("! ALDL buf too small");
        return 0;
    }
    // IMPROVE: can we do this in place on result_buf?
    byte receive_buf[ALDL_MAX_MESSAGE_SIZE];
    memset(receive_buf, 0x00, sizeof(receive_buf));
    memset(result_buf, 0x00, sizeof(result_buf));

    // assemble a mode 3 request
    byte mode3_request[16];
    mode3_request[0] = 0xF4;
    mode3_request[1] = 1 + (2*request_len) + 85;
    mode3_request[2] = 0x03;
    for (int i = 0; i < request_len; i++)
    {
        mode3_request[3 + 2*i] = request[i] >> 8;
        mode3_request[4 + 2*i] = request[i] & 0xFF;
    }
    unsigned int checksum_index = 3 + 2*request_len;
    mode3_request[checksum_index] = ALDL_CalculateChecksum(mode3_request, checksum_index);
    mode3_request[checksum_index+1] = 0;
    SerialDebug.write("mode3 message:");
    for (int i = 0; i < checksum_index + 2; i++)
    {
        SerialDebug.print("0x");
        if (mode3_request[i] < 16)
        {
            SerialDebug.print("0");
        }
        SerialDebug.print(mode3_request[i], HEX);
        SerialDebug.print(",");
    }
    SerialDebug.println();
    
    SerialALDL.write(mode3_request, checksum_index + 1);
    SerialALDL.flush();
    // read data
    // first look for the header -- it has to be 0xF4 (header) 0x95 (XXX: why?) 0x01
    // we attempt to read at least as many bytes as the request plus a few more, that
    // way if we see the echo of our previous message we read past it.
    // IMPROVE: scanning function here read and scan until we see 0xF4 0x95 0x01.
    //          we also need to figure out why it's 0x95 so we can calculate that.
    //          it shoudl be the length + 85 decimal but that doesn't agree.
    // header for message from ECU
    const int START_SEQ_LENGTH = 3;
    byte resp_header[3];
    resp_header[0] = 0xF4;
    // XXX: why 0x55
    resp_header[1] = request_len + 1 + 0x55;
    resp_header[2] = 0x03;

    // set timeout to just the time necessary to fill the buf if the bus is busy enough
    SerialALDL.setTimeout(ALDL_MAX_MESSAGE_SIZE * 0.125);
    int readlen = SerialALDL.readBytes(receive_buf,
                                       ALDL_MAX_MESSAGE_SIZE);
    // scan the received data for the start sequence
    bool match = 0;
    int i = 0;
    for (i = 0; i < readlen - START_SEQ_LENGTH; i++)
    {
        if (memcmp(receive_buf + i, resp_header, START_SEQ_LENGTH) == 0)
        {
            match = 1;
            break;
        }
    }
    // FIXME: this is not the correct length to copy. it should stop at end of message!!!
    unsigned int message_len = ALDL_MAX_MESSAGE_SIZE;
    // byte index 1 in the header is the size plus 0x52.
    unsigned int len_from_header = receive_buf[i + 1] - 0x52;
    if (len_from_header < ALDL_MAX_MESSAGE_SIZE)
    {
        message_len = len_from_header;
    }
    memcpy(result_buf, receive_buf + i, message_len);
    if (!match)
    {
        SerialDebug.println("! no header match");
    }
#ifdef DEBUG_LOG_ALDL
    if (!match){
      DebugPrintBuf(receive_buf, ALDL_MAX_MESSAGE_SIZE);
    } else {
      DebugPrintBuf(result_buf, message_len);
    }
#endif
    if (!match){
        return 1;
    }
    // verify data integrity
    if (!ALDL_VerifyChecksum(result_buf, message_len))
    {
        SerialDebug.println("! ALDL checksum error");
        return 1;
    }
#ifdef DEBUG_LOG_ALDL
    SerialDebug.println("message OK");
#endif
    return 0;
}


void CAN_Init()
{
    // init can bus : arbitration bitrate = 500k, data bitrate = 1M
    while (0 != CAN.begin(CAN_BAUD_RATE))
    {
        SerialDebug.println("! CAN init fail, retry");
        delay(100);
    }
    CAN.setMode(CAN_NORMAL_MODE);

    SerialDebug.println("CAN init ok");

    byte mode = CAN.getMode();
    SerialDebug.print("CAN mode = ");
    SerialDebug.println(mode);
}

void CAN_SendBatch(unsigned long base_pid, byte *data, int len)
{
    // send data to CAN bus. split into 8 bytes per frame.
    unsigned long can_frame_pid = base_pid;
    for (int i = 0; i < len; i += CAN_MAX_FRAME_SIZE)
    {
        unsigned long frame_len = min(CAN_MAX_FRAME_SIZE, len - i);
        for (int j = 0; j < frame_len; j++)
        {
            can_message[j] = data[i + j];
        }
        CAN.sendMsgBuf(can_frame_pid, USE_EXTENDED_PID, frame_len, can_message);
        can_frame_pid++;
        delay(10);
    }
}

void CAN_SendMapped(unsigned long base_pid, byte *data, int data_len, const byte_map_t *map)
{
    byte can_data[8] = {0};
    unsigned long can_pid_offset = 0;
    int frame_length = 0;
    unsigned int map_index = 0;
    // send all values from the map. the last map entry has a label of numeric 0.
    while (map[map_index].can_pid_offset != 255)
    {
        const byte_map_t *entry = &map[map_index];
        map_index += 1;
        // when the PID offset changes, we go to the next frame
        if (entry->can_pid_offset != can_pid_offset)
        {
            // end of this frame, send frame if any data
            if (frame_length > 0)
            {
                CAN_SendBatch(base_pid + can_pid_offset, can_data, frame_length);
            }
            // start working on the new frame
            can_pid_offset = entry->can_pid_offset;
            frame_length = 0;
            memset(can_data, 0, 8);
        }
        // pack the next ALDL data element into the CAN frame
        int can_byte_offset = entry->can_byte_offset;

        for (int j = 0; j < entry->num_bytes; j++)
        {
            unsigned int aldl_data_index = entry->aldl_byte_offset + j;
            if (aldl_data_index >= data_len)
            {
                SerialDebug.println("! map index exceeds data");
                continue;
            }
            byte aldl_byte = (data[entry->aldl_byte_offset + j]);
            can_data[can_byte_offset + j] = aldl_byte;
            if (can_byte_offset + j + 1 > frame_length)
            {
                frame_length = can_byte_offset + j + 1;
            }
        }
    }
    // end of map, send last batch
    if (frame_length > 0)
    {
        CAN_SendBatch(base_pid + can_pid_offset, can_data, frame_length);
    }
    return;
}

void setup()
{
    ALDLMask = &aldl_DF;
    SerialDebug_Init();
    ALDL_Init();
    CAN_Init();
    // give the ECM time to boot up
    delay(500);
}


#define ERROR_NONE           0
#define ERROR_BUF_TOO_SMALL  1
#define ERROR_ALDL_READ_FAIL 2


void loop()
{
    static int iteration = 0;
    int error = ERROR_NONE;
    if (ALDL_MAX_MESSAGE_SIZE < ALDLMask->mode1_response_length)
    {
        SerialDebug.println("! ALDL buffer too small");
        error = 1;
    }
    if (iteration % 5 == 0)
    {
        // periodically take control of the bus to disable ECU chatter
        ALDL_SilenceBus();
    }
    int retval = ALDL_ReadMode1(aldl_raw, ALDL_MAX_MESSAGE_SIZE);
    if (retval != 0)
    {
        SerialDebug.println("! ALDL read fail");
        error = 1;
    }
    if (error == ERROR_NONE){
        CAN_SendMapped(CAN_BASE_PID,
                    aldl_raw + ALDLMask->mode1_data_offset,
                    ALDL_MAX_MESSAGE_SIZE - ALDLMask->mode1_data_offset,
                    ALDLMask->mode1_map);
    }
    /*
    uint16_t request[] = {
        // modew1
        // bit1=vehicle moving, bit5=a/c clutch enabled, bit7=engine running
        0x0028,
        // lccpmw
        // bit0=shift light on, bit1=malfunctions have occurred
        0x00A7,
    };
    int retval = ALDL_ReadMode3(aldl_raw, ALDL_MAX_MESSAGE_SIZE, request, 2);
    if (retval != 0){
      delay(1000);
      return;
    }*/
    byte status_frame[8] = {0};
    status_frame[0] = 0x01;
    status_frame[1] = error;
    CAN.sendMsgBuf(CAN_STATUS_PID, USE_EXTENDED_PID, 2, status_frame);
    if (error != ERROR_NONE){
        delay(250);
    } else {
        delay(50);
    }
    iteration += 1;
}
