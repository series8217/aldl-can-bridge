#include "definitions/df.h"
#include <string.h>
#include "linuxaldl.h"

// ==========================================
//
//   DF CODE MASK DEFINITION
// Engine: 1991-1993 3.4 DOHC V6 (LQ1) Vin "X"
//
// aldl_definition_table entry: aldl_DF
// mode1 definition table: aldl_DF_mode1
// ===========================================
// FIXME: we can use this to generate a DBC file.
//        however it is not needed for packing to CAN and is
//        too big to actually fit on the arduino.
//        so this should be kept on the side for now.
//        also maybe use a more standard format like whatever
//        the scan tools use.
//        here, we only need to know how many bytes each field is,
//        so we can pack it into the CAN message without splitting
//        fields across multiple CAN frames.
const byte_def_t aldl_DF_mode1[]=
	{
			_DEF_SEP("---Basic Data---"),
			{"Engine RPM",			11,	8,	0,	25.0,		0.0,	"RPM"},
			{"Throttle Position", 	10, 8, 	0, 	0.003906, 	0.00, 	"%"},
			{"Vehicle Speed", 		17, 8, 	0, 	1.0, 		0.0, 	"MPH"},
			{"Engine Airflow", 		37, 8, 	0, 	1.0, 		0.0, 	"gm/sec"},
			{"Coolant Temp", 		7, 	8, 	0, 	1.35, 		-40.0, 	"Deg F"},
			{"Intake Air Temp",		30, 8,	0,	1.0,		0.0,	"adc"},
			{"MAP",					29,	8,	0,	0.369,		10.354,	"kPa"},
			_DEF_SEP("----Fuel----"),
			{"Desired AFR",			41,	8,	0,	0.100,		0.0,	"A/F"},
			{"Narrowband O2",		19,	8,	0,	4.42,		0.0,	"mV"},
			{"Final Base Pulse Width", 42, 16, 0, 0.015259, 0.0,	"mSec"},
			{"Current BLM Cell",	23,	8,	0,	1.0,		0.0,	""},
			{"BLM",					22,	8,	0,	1.0,		0.0,	"counts"},
			{"Integrator",			24,	8,	0,	1.0,		0.0,	"counts"},
			{"Base Pulse Fine Corr.",21,8,	0,	1.0,		0.0,	"counts"},
			{"BLM Cell 0 Timer",	36,	8,	0,	1.0,		0.0,	"counts"},
			_DEF_SEP("--Ignition--"),
			{"Knock Events",		51,	8,	0,	1.0,		0.0,	"counts"},
			{"Spark Advance",		40, 8,	0,	0.351560,	0.0,	"degrees"},
			{"Knock Retard",		46,	8,	0,	0.175781,	0.0,	"degrees"},
			_DEF_SEP("--Accessory Data--"),
			{"PROM ID",				1,	16,	0,	1.0,		0.0,	"ID"},
			{"TPS Voltage",			9,	8,	0,	0.019531,	0.0,	"volts"},
			{"IAC Steps",			25, 8,  0,  1.0,		0.0,	"steps"},
			{"IAC Min Position",	22, 8,  0,	1.0,		0.0,	"steps"},
			{"Barometric Pressure",	28,	8,	0,	0.369,		10.3542,"kPa"},
			{"Engine Run Time",		48,	16,	0,	1.0,		0.0,	"secs"},
			{"Catalytic Conv Temp",	50,	8,	0,	3.0,		300.0,	"Deg C"},
			{"Fuel Pump Relay Volts",31,8,	0,	0.1,		0.0,	"volts"},
			{"O2 Cross-Count",		20,	8,	0,	1.0,		0.0,	"counts"},
			{"Desired Idle Speed",	27, 8, 	0,	12.5,		0.0,	"RPM"},
			{"Battery Voltage",		34,	8,	0,	0.1,		0.0,	"volts"},
			{"CCP Duty Cycle",		45,	8,	0,	0.390650,	0.0,	"% CCP"},
			{"RPM/MPH",				47, 8,	0,	1.0,		0.0,	"RPM/MPH"},
			{"A/C Pressure Sensor",	33,	8,	0,	1.0,		0.0,	"A/D Counts"},
			{"Corrosivity Sensor",	44,	8,	0,	0.0196,		0.0,	"volts"},
			LINUXALDL_MODE1_END_DEF
	};

const aldl_definition_t aldl_DF = { .name="91-93 3.4 DOHC LQ1 ($DF)",
							.mode1_request={0xF4, 0x57, 0x01, 0x00, 0xB4},
							.mode1_request_length=5,
							.mode1_response_length=67,
							.mode1_data_length=63,
							.mode1_data_offset=3,
							.mode1_def=aldl_DF_mode1,
							.mode8_request={0xF4, 0x56, 0x08, 0xAE},
							.mode8_request_length=4,
							.mode9_request={0xF4, 0x56, 0x09, 0xAD},
							.mode9_request_length=4
						};