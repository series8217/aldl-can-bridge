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
// #define DEBUG_LOG_ALDL

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

#define ALDL_MAX_MESSAGE_SIZE 128
#define CAN_MAX_FRAME_SIZE 8

#define CAN_BASE_PID 0x380

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

int ALDL_Read(byte *aldl_raw, int len)
{
    if (len < ALDLMask->mode1_response_length)
    {
        SerialDebug.println("! ALDL buf too small");
        return 0;
    }
    // IMPROVE: can we do this in place on aldl_raw?
    byte receive_buf[ALDL_MAX_MESSAGE_SIZE];
    memset(receive_buf, 0xFF, sizeof(receive_buf));
    memset(aldl_raw, 0x00, sizeof(aldl_raw));

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
    int readlen = SerialALDL.readBytes(receive_buf,
                                       ALDL_MAX_MESSAGE_SIZE);
    // scan the received data for the start sequence
    bool match = 0;
    int i = 0;
    for (i = 0; i < readlen - START_SEQ_LENGTH; i++)
    {
        if (memcmp(receive_buf + i, mode1_resp_header, START_SEQ_LENGTH) == 0)
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
    memcpy(aldl_raw, receive_buf + i, message_len);
    if (!match)
    {
        SerialDebug.println("! no header match");
    }
#ifdef DEBUG_LOG_ALDL
    // print the data as hex
    for (int i = 0; i < message_len; i++)
    {
        SerialDebug.print("0x");
        if (aldl_raw[i] < 16)
        {
            SerialDebug.print("0");
        }
        SerialDebug.print(aldl_raw[i], HEX);
        SerialDebug.print(",");
        if (i % 8 == 7)
        {
            SerialDebug.println();
        }
    }
    SerialDebug.println();
#endif
    if (message_len < ALDLMask->mode1_response_length - 3)
    {
        SerialDebug.println("! ALDL message len wrong");
    }
    // verify data integrity
    if (!ALDL_VerifyChecksum(aldl_raw, message_len))
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

void CAN_SendBatch(int base_pid, byte *data, int len)
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
        CAN.sendMsgBuf(can_frame_pid, 0, frame_len, can_message);
        can_frame_pid++;
        delay(10);
    }
}

void CAN_SendMapped(int base_pid, byte *data, int data_len, const byte_map_t *map)
{
    byte can_data[8] = {0};
    int can_pid_offset = 0;
    int frame_length = 0;
    unsigned int map_index = 0;
    // send all values from the map. the last map entry has a label of numeric 0.
    while (map[map_index].label != 0)
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
    delay(2000);
}

void loop()
{
    static int iteration = 0;
    if (ALDL_MAX_MESSAGE_SIZE < ALDLMask->mode1_response_length)
    {
        SerialDebug.println("! ALDL buffer too small");
        delay(1000);
        return;
    }
    if (iteration % 5 == 0)
    {
        // periodically take control of the bus to disable ECU chatter
        ALDL_SilenceBus();
    }
    int retval = ALDL_Read(aldl_raw, ALDL_MAX_MESSAGE_SIZE);
    if (retval != 0)
    {
        SerialDebug.println("! ALDL read fail");
        delay(1000);
        return;
    }
    CAN_SendMapped(CAN_BASE_PID,
                   aldl_raw + ALDLMask->mode1_data_offset,
                   ALDL_MAX_MESSAGE_SIZE - ALDLMask->mode1_data_offset,
                   ALDLMask->mode1_map);
    delay(100);
    iteration += 1;
    SerialDebug.print(".");
}
