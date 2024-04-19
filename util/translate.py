#!/usr/bin/env python3

"""Translate the ALDL definition to a CAN DBC template."""


ALDL_DEF_DF = [
    ("Engine RPM", 11, 8, 0, 25.0, 0.0, "RPM"),
    ("Throttle Position", 10, 8, 0, 0.003906, 0.00, "%"),
    ("Vehicle Speed", 17, 8, 0, 1.0, 0.0, "MPH"),
    ("Engine Airflow", 37, 8, 0, 1.0, 0.0, "gm/sec"),
    ("Coolant Temp", 7, 8, 0, 1.35, -40.0, "Deg F"),
    ("Intake Air Temp", 30, 8, 0, 1.0, 0.0, "adc"),
    ("MAP", 29, 8, 0, 0.369, 10.354, "kPa"),
    ("Desired AFR", 41, 8, 0, 0.100, 0.0, "A/F"),
    ("Narrowband O2", 19, 8, 0, 4.42, 0.0, "mV"),
    ("Final Base Pulse Width", 42, 16, 0, 0.015259, 0.0, "mSec"),
    ("Current BLM Cell", 23, 8, 0, 1.0, 0.0, ""),
    ("BLM", 22, 8, 0, 1.0, 0.0, "counts"),
    ("Integrator", 24, 8, 0, 1.0, 0.0, "counts"),
    ("Base Pulse Fine Corr.", 21, 8, 0, 1.0, 0.0, "counts"),
    ("BLM Cell 0 Timer", 36, 8, 0, 1.0, 0.0, "counts"),
    ("Knock Events", 51, 8, 0, 1.0, 0.0, "counts"),
    ("Spark Advance", 40, 8, 0, 0.351560, 0.0, "degrees"),
    ("Knock Retard", 46, 8, 0, 0.175781, 0.0, "degrees"),
    ("PROM ID", 1, 16, 0, 1.0, 0.0, "ID"),
    ("TPS Voltage", 9, 8, 0, 0.019531, 0.0, "volts"),
    ("IAC Steps", 25, 8, 0, 1.0, 0.0, "steps"),
    ("IAC Min Position", 22, 8, 0, 1.0, 0.0, "steps"),
    ("Barometric Pressure", 28, 8, 0, 0.369, 10.3542, "kPa"),
    ("Engine Run Time", 48, 16, 0, 1.0, 0.0, "secs"),
    ("Catalytic Conv Temp", 50, 8, 0, 3.0, 300.0, "Deg C"),
    ("Fuel Pump Relay Volts", 31, 8, 0, 0.1, 0.0, "volts"),
    ("O2 Cross-Count", 20, 8, 0, 1.0, 0.0, "counts"),
    ("Desired Idle Speed", 27, 8, 0, 12.5, 0.0, "RPM"),
    ("Battery Voltage", 34, 8, 0, 0.1, 0.0, "volts"),
    ("CCP Duty Cycle", 45, 8, 0, 0.390650, 0.0, "% CCP"),
    ("RPM/MPH", 47, 8, 0, 1.0, 0.0, "RPM/MPH"),
    ("A/C Pressure Sensor", 33, 8, 0, 1.0, 0.0, "A/D Counts"),
    ("Corrosivity Sensor", 44, 8, 0, 0.0196, 0.0, "volts"),
]


for name, byte_offset, num_bits, _, factor, offset, units in ALDL_DEF_DF:
    # remove bad chars from name
    name = name.replace(" ", "")
    name = name.replace("/", "")
    name = name.replace(".", "")
    print(
        f'\tSG_ {name:18}:\t{16*(byte_offset-1)}|{num_bits}@0+\t({factor},{offset})\t[0|0]\t"{units}"'
    )
