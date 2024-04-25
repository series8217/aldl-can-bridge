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
// XXX: FIXME: all these byte offsets are one less than in the tunerpro definition ADS file.
const byte_map_t aldl_DF_mode1_byte_map[] =
	{
		// frames must be in order, because the data is packed and sent
		// in order.

		// aldl_byte_offset, num_bytes, frame, can_pid_offset
		// frame 0
		/* EngineRPM */ {10, 1, 0, 0},
		/* EngineTPS */ {9, 1, 0, 1},
		/* VehicleSpeed */ {16, 1, 0, 2},
		/* EngineAirflow */ {36, 1, 0, 3},
		/* CoolantTemp */ {6, 1, 0, 4},
		/* IntakeAirTemp */ {29, 1, 0, 5},
		/* MAP */ {28, 1, 0, 6},

		// frame 1
		/* DesiredAFR */ {40, 1, 1, 0},
		/* NarrowbandO2 */ {18, 1, 1, 1},
		/* FinalBasePulseWidth */ {41, 2, 1, 2},

		// frame 2
		/* CurrentBLMCell */ {22, 1, 2, 0},
		/* BLM */ {21, 1, 2, 1},
		/* Integrator */ {23, 1, 2, 2},
		/* BasePulseFineCorr */ {20, 1, 2, 3},
		/* BLMCell0Timer */ {35, 1, 2, 4},

		// frame 3
		/* KnockEvents */ {50, 1, 3, 0},
		/* SparkAdvance */ {37, 2, 3, 1},
		/* KnockRetard */ {45, 1, 3, 3},

		// frame 4
		/* TPSVoltage */ {8, 1, 4, 0},
		/* IACSteps */ {24, 1, 4, 1},
		/* IACMinPosition */ {21, 1, 4, 2},
		/* BarometricPressure */ {27, 1, 4, 3},
		/* EngineRunTime */ {47, 2, 4, 4},
		/* CatalyticConvTemp */ {49, 1, 4, 6},

		// frame 5
		/* FuelPumpRelayVolts */ {30, 1, 5, 0},
		/* O2CrossCount */ {19, 1, 5, 1},
		/* DesiredIdleSpeed */ {26, 1, 5, 2},
		/* BatteryVoltage */ {33, 1, 5, 3},
		/* CCPDutyCycle */ {44, 1, 5, 4},
		/* RPMMPH */ {46, 1, 5, 5},
		/* ACPressureSensor */ {32, 1, 5, 6},
		/* CorrosivitySensor */ {43, 1, 5, 7},

		// frame 6
		/* PROMID */ {0, 2, 6, 0},

		// frame 7
		/* malfunction flags byte 1 */ {2, 1, 7, 0},
		/* malfunction flags byte 2 */ {3, 1, 7, 1},
		/* malfunction flags byte 3 */ {4, 1, 7, 2},
		/* malfunction flags byte 4 */ {5, 1, 7, 3},

		/* end of map */ {255, 255, 255, 255}};

const aldl_definition_t aldl_DF = {.name = "91-93 3.4 DOHC LQ1 ($DF)",
								   // message ID byte 0xF4, length 2 (+55), mode 1, checksum
								   .mode1_request = {0xF4, 0x57, 0x01, 0x00, 0xB4},
								   .mode1_request_length = 5,
								   .mode1_response_length = 67,
								   // data is 63 bytes of the response
								   .mode1_data_length = 63,
								   // data starts at byte id 3
								   .mode1_data_offset = 3,
								   .mode1_map = aldl_DF_mode1_byte_map,
								   .mode8_request = {0xF4, 0x56, 0x08, 0xAE},
								   .mode8_request_length = 4,
								   .mode9_request = {0xF4, 0x56, 0x09, 0xAD},
								   .mode9_request_length = 4};
