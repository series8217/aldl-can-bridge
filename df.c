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
const byte_map_t aldl_DF_mode1_byte_map[] =
	{
		// label, aldl_byte_offset, num_bytes, can_pid_offset, can_byte_offset
		// frames must be in order, because the data is packed and sent
		// in order.
		// frame 0
		{"EngineRPM", 11, 1, 0, 0},
		{"EngineTPS", 10, 1, 0, 8},
		{"VehicleSpeed", 17, 1, 0, 16},
		{"EngineAirflow", 37, 1, 0, 24},
		{"CoolantTemp", 7, 1, 0, 32},
		{"IntakeAirTemp", 30, 1, 0, 40},
		{"MAP", 29, 1, 0, 48},
		// frame 1
		{"DesiredAFR", 41, 1, 1, 8},
		{"NarrowbandO2", 19, 1, 1, 8},
		{"FinalBasePulseWidth", 42, 2, 1, 16},
		{"CurrentBLMCell", 23, 1, 1, 32},
		{"BLM", 22, 1, 1, 40},
		{"Integrator", 24, 1, 1, 48},
		{"BasePulseFineCorr", 21, 1, 2, 0},
		// frame 2
		{"BLMCell0Timer", 36, 1, 2, 8},
		{"KnockEvents", 51, 1, 2, 16},
		{"SparkAdvance", 40, 1, 2, 24},
		{"KnockRetard", 46, 1, 2, 32},
		{"PROMID", 1, 2, 2, 40},
		// frame 3
		{"TPSVoltage", 9, 1, 3, 0},
		{"IACSteps", 25, 1, 3, 8},
		{"IACMinPosition", 22, 1, 3, 16},
		{"BarometricPressure", 28, 1, 3, 24},
		{"EngineRunTime", 48, 2, 3, 32},
		{"CatalyticConvTemp", 50, 1, 3, 48},
		// frame 4
		{"FuelPumpRelayVolts", 31, 1, 4, 0},
		{"O2CrossCount", 20, 1, 4, 8},
		{"DesiredIdleSpeed", 27, 1, 4, 16},
		{"BatteryVoltage", 34, 1, 4, 24},
		{"CCPDutyCycle", 45, 1, 4, 32},
		{"RPMMPH", 47, 1, 4, 40},
		{"ACPressureSensor", 33, 1, 4, 48},
		// frame 5
		{"CorrosivitySensor", 44, 1, 5, 0},
		{0, 0, 0, 0, 0}};

const aldl_definition_t aldl_DF = {.name = "91-93 3.4 DOHC LQ1 ($DF)",
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
