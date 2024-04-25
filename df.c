#include <string.h>
#include "definition.h"

// ==========================================
//
//   DF CODE MASK DEFINITION
// Engine: 1991-1993 3.4 DOHC V6 (LQ1) Vin "X"
//
// aldl_definition_table entry: aldl_DF
// mode1 definition table: aldl_DF_mode1
// ===========================================

// XXX: check for correctness against DF mask
// these byte offsets match the tunerpro definition ADS file which has an off by one error
// in that it's data reads from the wrong byte offset in the mode 1 response. This is why
// we use a data offset of 2 instead of 3 in the definition.
const byte_map_t aldl_DF_mode1_byte_map[] =
	{
		// frames must be in order, because the data is packed and sent
		// in order.

		// aldl_byte_offset, num_bytes, frame, can_pid_offset
		// frame 0
		/* EngineRPM */ {11, 1, 0, 0},
		/* EngineTPS */ {10, 1, 0, 1},
		/* VehicleSpeed */ {17, 1, 0, 2},
		/* EngineAirflow */ {37, 1, 0, 3},
		/* CoolantTemp */ {7, 1, 0, 4},
		/* IntakeAirTemp */ {30, 1, 0, 5},
		/* MAP */ {29, 1, 0, 6},

		// frame 1
		/* DesiredAFR */ {41, 1, 1, 0},
		/* NarrowbandO2 */ {19, 1, 1, 1},
		/* FinalBasePulseWidth */ {42, 2, 1, 2},

		// frame 2
		/* CurrentBLMCell */ {23, 1, 2, 0},
		/* BLM */ {22, 1, 2, 1},
		/* Integrator */ {24, 1, 2, 2},
		/* BasePulseFineCorr */ {21, 1, 2, 3},
		/* BLMCell0Timer */ {36, 1, 2, 4},

		// frame 3
		/* KnockEvents */ {51, 1, 3, 0},
		/* SparkAdvance */ {38, 2, 3, 1},
		/* KnockRetard */ {46, 1, 3, 3},

		// frame 4
		/* TPSVoltage */ {9, 1, 4, 0},
		/* IACSteps */ {25, 1, 4, 1},
		/* IACMinPosition */ {22, 1, 4, 2},
		/* BarometricPressure */ {28, 1, 4, 3},
		/* EngineRunTime */ {48, 2, 4, 4},
		/* CatalyticConvTemp */ {50, 1, 4, 6},

		// frame 5
		/* FuelPumpRelayVolts */ {31, 1, 5, 0},
		/* O2CrossCount */ {20, 1, 5, 1},
		/* DesiredIdleSpeed */ {27, 1, 5, 2},
		/* BatteryVoltage */ {34, 1, 5, 3},
		/* CCPDutyCycle */ {45, 1, 5, 4},
		/* RPMMPH */ {47, 1, 5, 5},
		/* ACPressureSensor */ {33, 1, 5, 6},
		/* CorrosivitySensor */ {44, 1, 5, 7},

		// frame 6
		/* PROMID */ {1, 2, 6, 0},

		// frame 7
		/* malfunction flags 1 (byte 3) */ {3, 1, 7, 0},
		/* malfunction flags 2 (byte 4) */ {4, 1, 7, 1},
		/* malfunction flags 3 (byte 5) */ {5, 1, 7, 2},
		/* malfunction flags 4 (byte 6) */ {6, 1, 7, 3},

		// frame 8
		/* status flags 1 (byte 18) */ {18, 1, 8, 0},
		/* status flags 2 (byte 52) */ {52, 1, 8, 1},
		/* status flags 3 (byte 53) */ {53, 1, 8, 2},
		/* status flags 4 (byte 58) */ {58, 1, 8, 3},
		/* status flags 5 (byte 59) */ {59, 1, 8, 4},
		/* status flags 6 (byte 60) */ {60, 1, 8, 5},
		/* status flags 7 (byte 62) */ {62, 1, 8, 6},
		/* status flags 8 (byte 63) */ {63, 1, 8, 7},

		/* end of map */ {255, 255, 255, 255}};

const aldl_definition_t aldl_DF = {.name = "91-93 3.4 DOHC LQ1 ($DF)",
								   // message ID byte 0xF4, length 2 (+55), mode 1, checksum
								   .mode1_request = {0xF4, 0x57, 0x01, 0x00, 0xB4},
								   .mode1_request_length = 5,
								   .mode1_response_length = 67,
								   // data is 63 bytes of the response
								   .mode1_data_length = 63,
								   // data starts at byte id 2 to use the offsets we have above.
								   // see the note about the tunerpro definition.
								   .mode1_data_offset = 2,
								   .mode1_map = aldl_DF_mode1_byte_map,
								   .mode8_request = {0xF4, 0x56, 0x08, 0xAE},
								   .mode8_request_length = 4,
								   .mode9_request = {0xF4, 0x56, 0x09, 0xAD},
								   .mode9_request_length = 4};
