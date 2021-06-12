EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L stepper_motor_analyzer:Pico U3
U 1 1 609C8FE2
P 4200 5700
F 0 "U3" H 3750 6900 50  0000 C CNN
F 1 "Pico" H 4200 6824 50  0001 C CNN
F 2 "stepper_motor_analyzer:Pico_smd_dedicated" V 4200 5700 50  0001 C CNN
F 3 "" H 4200 5700 50  0001 C CNN
	1    4200 5700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 609CD7B7
P 5150 6800
F 0 "#PWR0101" H 5150 6550 50  0001 C CNN
F 1 "GND" H 5155 6627 50  0000 C CNN
F 2 "" H 5150 6800 50  0001 C CNN
F 3 "" H 5150 6800 50  0001 C CNN
	1    5150 6800
	1    0    0    -1  
$EndComp
$Comp
L stepper_motor_analyzer:Connector_Conn_01x04_Male J1
U 1 1 609CDEF6
P 1300 1550
F 0 "J1" H 1400 1750 50  0000 C CNN
F 1 "Connector_Conn_01x04_Male" H 1408 1740 50  0001 C CNN
F 2 "stepper_motor_analyzer:connector_4pins_horizontal" H 1300 1550 50  0001 C CNN
F 3 "" H 1300 1550 50  0001 C CNN
	1    1300 1550
	1    0    0    1   
$EndComp
$Comp
L stepper_motor_analyzer:Connector_Conn_01x40_Female FPC1
U 1 1 609CEBE2
P 9100 3250
F 0 "FPC1" H 9050 1050 50  0000 L CNN
F 1 "Connector_Conn_01x40_Female" H 8300 700 50  0001 L CNN
F 2 "stepper_motor_analyzer:JUSHUO-AFC07-S40ECC-00_40P_05mm_flipped" H 9100 3250 50  0001 C CNN
F 3 "" H 9100 3250 50  0001 C CNN
F 4 "C11097" H 9100 3250 50  0001 C CNN "LCSC"
	1    9100 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 4950 5150 4950
Wire Wire Line
	5150 4950 5150 5450
Wire Wire Line
	4900 5950 5150 5950
Connection ~ 5150 5950
Wire Wire Line
	5150 5950 5150 6350
Wire Wire Line
	4900 6450 5150 6450
Connection ~ 5150 6450
Wire Wire Line
	5150 6450 5150 6800
Wire Wire Line
	4900 5450 5150 5450
Connection ~ 5150 5450
Wire Wire Line
	5150 5450 5150 5950
$Comp
L power:GND #PWR0103
U 1 1 609E61CE
P 3300 6800
F 0 "#PWR0103" H 3300 6550 50  0001 C CNN
F 1 "GND" H 3305 6627 50  0000 C CNN
F 2 "" H 3300 6800 50  0001 C CNN
F 3 "" H 3300 6800 50  0001 C CNN
	1    3300 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 4950 3300 4950
Wire Wire Line
	3300 4950 3300 5450
Wire Wire Line
	3500 5450 3300 5450
Connection ~ 3300 5450
Wire Wire Line
	3300 5450 3300 5950
Wire Wire Line
	3500 5950 3300 5950
Connection ~ 3300 5950
Wire Wire Line
	3300 5950 3300 6450
Wire Wire Line
	3500 6450 3300 6450
Connection ~ 3300 6450
Wire Wire Line
	3300 6450 3300 6800
Wire Wire Line
	4900 5550 5650 5550
Wire Wire Line
	4900 5650 5650 5650
Text Label 5650 5550 2    50   ~ 0
CHB
Text Label 5650 5650 2    50   ~ 0
CHA
Wire Wire Line
	3500 4750 2800 4750
Wire Wire Line
	3500 5050 2800 5050
Wire Wire Line
	3500 5250 2800 5250
Wire Wire Line
	3500 5350 2800 5350
Wire Wire Line
	3500 5550 2800 5550
Wire Wire Line
	3500 5650 2800 5650
Wire Wire Line
	3500 5750 2800 5750
Wire Wire Line
	3500 5850 2800 5850
Wire Wire Line
	3500 6050 2800 6050
Wire Wire Line
	3500 6150 2800 6150
Wire Wire Line
	3500 6250 2800 6250
Wire Wire Line
	3500 6350 2800 6350
Wire Wire Line
	3500 6650 2800 6650
NoConn ~ 4900 4750
NoConn ~ 4900 4850
Text Label 2800 5050 0    50   ~ 0
TFT_DC
Text Label 5650 5350 2    50   ~ 0
~TFT_WR
Text Label 2800 5250 0    50   ~ 0
SDA
Text Label 2800 5350 0    50   ~ 0
SCL
Text Label 2800 5550 0    50   ~ 0
D0
Text Label 2800 5650 0    50   ~ 0
D1
Text Label 2800 5750 0    50   ~ 0
D2
Text Label 2800 5850 0    50   ~ 0
D3
Text Label 2800 6050 0    50   ~ 0
D4
Text Label 2800 6150 0    50   ~ 0
D5
Text Label 2800 6250 0    50   ~ 0
D6
Text Label 2800 6650 0    50   ~ 0
TFT_BL
$Comp
L power:GND #PWR0104
U 1 1 60A0E66B
P 4300 1050
F 0 "#PWR0104" H 4300 800 50  0001 C CNN
F 1 "GND" H 4305 877 50  0000 C CNN
F 2 "" H 4300 1050 50  0001 C CNN
F 3 "" H 4300 1050 50  0001 C CNN
	1    4300 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 1450 3250 1450
$Comp
L power:GND #PWR0105
U 1 1 60A1DF96
P 3850 2050
F 0 "#PWR0105" H 3850 1800 50  0001 C CNN
F 1 "GND" H 3855 1877 50  0000 C CNN
F 2 "" H 3850 2050 50  0001 C CNN
F 3 "" H 3850 2050 50  0001 C CNN
	1    3850 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 2000 3850 2050
Text Label 4750 1500 2    50   ~ 0
CHB
Wire Wire Line
	3850 900  3850 1000
Wire Wire Line
	3950 1000 3850 1000
Connection ~ 3850 1000
Wire Wire Line
	3850 1000 3850 1200
Text Label 5650 5150 2    50   ~ 0
+3V3
Wire Wire Line
	4900 5150 5650 5150
Wire Wire Line
	4250 1000 4300 1000
Wire Wire Line
	4300 1000 4300 1050
$Comp
L power:GND #PWR0107
U 1 1 60A384E4
P 5950 1300
F 0 "#PWR0107" H 5950 1050 50  0001 C CNN
F 1 "GND" H 5955 1127 50  0000 C CNN
F 2 "" H 5950 1300 50  0001 C CNN
F 3 "" H 5950 1300 50  0001 C CNN
	1    5950 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 900  5950 950 
Wire Wire Line
	5950 1250 5950 1300
$Comp
L stepper_motor_analyzer:Connector_Conn_01x04_Male J2
U 1 1 60A3C8AB
P 1300 2850
F 0 "J2" H 1400 3050 50  0000 C CNN
F 1 "Connector_Conn_01x04_Male" H 1400 3250 50  0001 C CNN
F 2 "stepper_motor_analyzer:connector_4pins_horizontal" H 1300 2850 50  0001 C CNN
F 3 "" H 1300 2850 50  0001 C CNN
	1    1300 2850
	1    0    0    1   
$EndComp
Wire Wire Line
	1500 2850 2300 2850
Wire Wire Line
	2150 1550 2150 2550
Wire Wire Line
	2150 2550 2300 2550
Wire Wire Line
	1500 2750 2000 2750
Wire Wire Line
	2000 2750 2000 1750
Wire Wire Line
	2000 1750 3250 1750
Wire Wire Line
	1500 1650 1700 1650
Wire Wire Line
	1700 1650 1700 2950
Wire Wire Line
	1700 2950 1500 2950
Wire Wire Line
	1500 1350 1850 1350
Wire Wire Line
	1850 1350 1850 2650
Wire Wire Line
	1850 2650 1500 2650
$Comp
L power:GND #PWR0108
U 1 1 60A49E54
P 2900 3150
F 0 "#PWR0108" H 2900 2900 50  0001 C CNN
F 1 "GND" H 2905 2977 50  0000 C CNN
F 2 "" H 2900 3150 50  0001 C CNN
F 3 "" H 2900 3150 50  0001 C CNN
	1    2900 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 3150 2900 3100
Wire Wire Line
	2900 2300 2900 2150
$Comp
L power:GND #PWR0110
U 1 1 60A5D6D2
P 3350 2200
F 0 "#PWR0110" H 3350 1950 50  0001 C CNN
F 1 "GND" H 3355 2027 50  0000 C CNN
F 2 "" H 3350 2200 50  0001 C CNN
F 3 "" H 3350 2200 50  0001 C CNN
	1    3350 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 2150 2900 2150
Connection ~ 2900 2150
Wire Wire Line
	2900 2150 2900 2050
Wire Wire Line
	3300 2150 3350 2150
Wire Wire Line
	3350 2150 3350 2200
Text Notes 1250 1450 2    70   ~ 0
B
Text Notes 1250 1650 2    70   ~ 0
A
Wire Wire Line
	1500 1550 2150 1550
Text Label 4750 2600 2    50   ~ 0
CHA
NoConn ~ 4900 5250
Text Notes 3550 3650 2    50   ~ 0
CURRENT SENSORS
$Comp
L stepper_motor_analyzer:+3V3 #PWR0102
U 1 1 60C23DAF
P 3850 900
F 0 "#PWR0102" H 3850 750 50  0001 C CNN
F 1 "+3V3" H 3865 1073 50  0000 C CNN
F 2 "" H 3850 900 50  0001 C CNN
F 3 "" H 3850 900 50  0001 C CNN
	1    3850 900 
	1    0    0    -1  
$EndComp
$Comp
L stepper_motor_analyzer:+3V3 #PWR0106
U 1 1 60C24CD5
P 2900 2050
F 0 "#PWR0106" H 2900 1900 50  0001 C CNN
F 1 "+3V3" H 2915 2223 50  0000 C CNN
F 2 "" H 2900 2050 50  0001 C CNN
F 3 "" H 2900 2050 50  0001 C CNN
	1    2900 2050
	1    0    0    -1  
$EndComp
$Comp
L stepper_motor_analyzer:+3V3 #PWR0109
U 1 1 60C25266
P 5950 900
F 0 "#PWR0109" H 5950 750 50  0001 C CNN
F 1 "+3V3" H 5965 1073 50  0000 C CNN
F 2 "" H 5950 900 50  0001 C CNN
F 3 "" H 5950 900 50  0001 C CNN
	1    5950 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 2250 8050 2250
Wire Wire Line
	8900 2750 8050 2750
NoConn ~ 8900 1350
NoConn ~ 8900 1450
NoConn ~ 8900 1550
NoConn ~ 8900 1650
$Comp
L stepper_motor_analyzer:+3V3 #PWR0116
U 1 1 60B0BD42
P 8500 1450
F 0 "#PWR0116" H 8500 1300 50  0001 C CNN
F 1 "+3V3" H 8450 1600 50  0000 C CNN
F 2 "" H 8500 1450 50  0001 C CNN
F 3 "" H 8500 1450 50  0001 C CNN
	1    8500 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 2150 8700 2150
Wire Wire Line
	8700 2150 8700 1750
Wire Wire Line
	8700 1750 8900 1750
Text Label 8050 2250 0    50   ~ 0
TFT_DC
Wire Wire Line
	8900 2350 8050 2350
Text Label 8050 2350 0    50   ~ 0
~TFT_WR
Wire Wire Line
	8500 2450 8900 2450
Wire Wire Line
	8500 1450 8500 1850
NoConn ~ 8900 2650
Text Label 8050 2750 0    50   ~ 0
~TFT_RST
Wire Wire Line
	8900 2850 8700 2850
$Comp
L power:GND #PWR0117
U 1 1 60CB883F
P 8700 5800
F 0 "#PWR0117" H 8700 5550 50  0001 C CNN
F 1 "GND" H 8705 5627 50  0000 C CNN
F 2 "" H 8700 5800 50  0001 C CNN
F 3 "" H 8700 5800 50  0001 C CNN
	1    8700 5800
	1    0    0    -1  
$EndComp
Connection ~ 8700 2150
Connection ~ 8700 2850
Connection ~ 8500 2450
Wire Wire Line
	8900 4950 8700 4950
Connection ~ 8700 4950
Wire Wire Line
	8900 5150 8500 5150
Wire Wire Line
	8500 1850 8900 1850
Connection ~ 8500 1850
Wire Wire Line
	8500 1850 8500 1950
Wire Wire Line
	8500 1950 8900 1950
Connection ~ 8500 1950
Wire Wire Line
	8500 1950 8500 2450
Wire Wire Line
	8900 5250 8700 5250
Connection ~ 8700 5250
Wire Wire Line
	8700 5250 8700 5800
Wire Wire Line
	8500 2450 8500 4550
$Comp
L power:GND #PWR0120
U 1 1 60F17E04
P 7900 5850
F 0 "#PWR0120" H 7900 5600 50  0001 C CNN
F 1 "GND" H 7905 5677 50  0000 C CNN
F 2 "" H 7900 5850 50  0001 C CNN
F 3 "" H 7900 5850 50  0001 C CNN
	1    7900 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 4850 8150 4900
Wire Wire Line
	8150 4850 8900 4850
Wire Wire Line
	7900 4750 7900 4900
Wire Wire Line
	7900 4750 8900 4750
Wire Wire Line
	7650 4650 7650 4900
Wire Wire Line
	7650 4650 8900 4650
Wire Wire Line
	7900 5200 7900 5300
Text Label 2800 6350 0    50   ~ 0
D7
Wire Wire Line
	8700 2850 8700 3750
Wire Wire Line
	8900 2950 8050 2950
Wire Wire Line
	8900 3050 8050 3050
Wire Wire Line
	8900 3150 8050 3150
Wire Wire Line
	8900 3250 8050 3250
Wire Wire Line
	8900 3350 8050 3350
Wire Wire Line
	8900 3450 8050 3450
Wire Wire Line
	8050 3550 8900 3550
Wire Wire Line
	8900 3650 8050 3650
Text Label 8050 2950 0    50   ~ 0
D0
Text Label 8050 3050 0    50   ~ 0
D1
Text Label 8050 3150 0    50   ~ 0
D2
Text Label 8050 3250 0    50   ~ 0
D3
Text Label 8050 3350 0    50   ~ 0
D4
Text Label 8050 3450 0    50   ~ 0
D5
Text Label 8050 3550 0    50   ~ 0
D6
Text Label 8050 3650 0    50   ~ 0
D7
NoConn ~ 4900 5050
$Comp
L stepper_motor_analyzer:Conn_01x06_Female FPC2
U 1 1 60A0E4CD
P 6300 3300
F 0 "FPC2" H 6550 3450 50  0000 L CNN
F 1 "FPC connector, 6 pins, 0.5mm, BOTTOM contacts" H 5750 2850 50  0001 L CNN
F 2 "stepper_motor_analyzer:JUSHUO-AFC01-S06FCC-00_6p_05mm" H 6300 3300 50  0001 C CNN
F 3 "~" H 6300 3300 50  0001 C CNN
F 4 "C262262" H 6300 3300 50  0001 C CNN "LCSC"
	1    6300 3300
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR0123
U 1 1 60A1EE83
P 5650 3000
F 0 "#PWR0123" H 5650 2750 50  0001 C CNN
F 1 "GND" H 5650 2850 50  0000 C CNN
F 2 "" H 5650 3000 50  0001 C CNN
F 3 "" H 5650 3000 50  0001 C CNN
	1    5650 3000
	1    0    0    -1  
$EndComp
$Comp
L stepper_motor_analyzer:+3V3 #PWR0124
U 1 1 60A1F3E7
P 5650 3500
F 0 "#PWR0124" H 5650 3350 50  0001 C CNN
F 1 "+3V3" H 5600 3650 50  0000 C CNN
F 2 "" H 5650 3500 50  0001 C CNN
F 3 "" H 5650 3500 50  0001 C CNN
	1    5650 3500
	1    0    0    -1  
$EndComp
$Comp
L stepper_motor_analyzer:ACS70331_soic8 U2
U 1 1 60AEB4AA
P 3900 1600
F 0 "U2" H 3250 2200 50  0000 C CNN
F 1 "ACS70331EOLCTR-2P5B3" H 3250 2050 50  0000 C CNN
F 2 "stepper_motor_analyzer:SOIC-8_3.9x4.9mm_P1.27mm" H 4300 1550 50  0001 L CIN
F 3 "" H 3900 1600 50  0001 C CNN
	1    3900 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3300 1400 3250 1400
Wire Wire Line
	3250 1400 3250 1450
Wire Wire Line
	3250 1500 3300 1500
Connection ~ 3250 1450
Wire Wire Line
	3250 1450 3250 1500
Wire Wire Line
	3300 1700 3250 1700
Wire Wire Line
	3250 1700 3250 1750
Wire Wire Line
	3250 1800 3300 1800
Connection ~ 3250 1750
Wire Wire Line
	3250 1750 3250 1800
Wire Wire Line
	2350 2500 2300 2500
Wire Wire Line
	2300 2500 2300 2550
Wire Wire Line
	2300 2600 2350 2600
Connection ~ 2300 2550
Wire Wire Line
	2300 2550 2300 2600
Wire Wire Line
	2350 2800 2300 2800
Wire Wire Line
	2300 2800 2300 2850
Wire Wire Line
	2300 2900 2350 2900
Connection ~ 2300 2850
Wire Wire Line
	2300 2850 2300 2900
$Comp
L stepper_motor_analyzer:ACS70331_soic8 U1
U 1 1 60B5D76B
P 2950 2700
F 0 "U1" H 2150 2050 50  0000 C CNN
F 1 "ACS70331EOLCTR-2P5B3" H 2200 2200 50  0000 C CNN
F 2 "stepper_motor_analyzer:SOIC-8_3.9x4.9mm_P1.27mm" H 3350 2650 50  0001 L CIN
F 3 "" H 2950 2700 50  0001 C CNN
	1    2950 2700
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 60A5D335
P 3150 2150
F 0 "C1" V 3100 2250 50  0000 L CNN
F 1 "0.1u" V 3100 2450 50  0000 L CNN
F 2 "stepper_motor_analyzer:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 3188 2000 50  0001 C CNN
F 3 "~" H 3150 2150 50  0001 C CNN
F 4 "C49678 " V 3150 2150 50  0001 C CNN "LCSC"
	1    3150 2150
	0    1    1    0   
$EndComp
$Comp
L Device:C C2
U 1 1 60A0EB39
P 4100 1000
F 0 "C2" V 3950 950 50  0000 L CNN
F 1 "0.1u" V 4050 1100 50  0000 L CNN
F 2 "stepper_motor_analyzer:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4138 850 50  0001 C CNN
F 3 "~" H 4100 1000 50  0001 C CNN
F 4 "C49678 " V 4100 1000 50  0001 C CNN "LCSC"
	1    4100 1000
	0    1    1    0   
$EndComp
$Comp
L Device:R_US R3
U 1 1 60C944CD
P 8150 5050
F 0 "R3" V 8050 5150 50  0000 L CNN
F 1 "8R2" V 8050 4950 50  0000 L CNN
F 2 "stepper_motor_analyzer:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 8190 5040 50  0001 C CNN
F 3 "~" H 8150 5050 50  0001 C CNN
F 4 "C25330" V 8150 5050 50  0001 C CNN "LCSC"
	1    8150 5050
	-1   0    0    1   
$EndComp
$Comp
L Device:R_US R2
U 1 1 60C9422D
P 7900 5050
F 0 "R2" V 7800 5150 50  0000 L CNN
F 1 "8R2" V 7800 4950 50  0000 L CNN
F 2 "stepper_motor_analyzer:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 7940 5040 50  0001 C CNN
F 3 "~" H 7900 5050 50  0001 C CNN
F 4 "C25330" V 7900 5050 50  0001 C CNN "LCSC"
	1    7900 5050
	-1   0    0    1   
$EndComp
$Comp
L Device:R_US R1
U 1 1 60C7B2FA
P 7650 5050
F 0 "R1" V 7550 5150 50  0000 L CNN
F 1 "8R2" V 7550 4950 50  0000 L CNN
F 2 "stepper_motor_analyzer:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 7690 5040 50  0001 C CNN
F 3 "~" H 7650 5050 50  0001 C CNN
F 4 "C25330" V 7650 5050 50  0001 C CNN "LCSC"
	1    7650 5050
	-1   0    0    1   
$EndComp
Wire Wire Line
	8700 2150 8700 2550
Wire Wire Line
	8900 2550 8700 2550
Connection ~ 8700 2550
Wire Wire Line
	8700 2550 8700 2850
Wire Wire Line
	6100 3400 5800 3400
Wire Wire Line
	6100 3300 5800 3300
Text Label 5800 3400 0    50   ~ 0
SDA
Text Label 5800 3300 0    50   ~ 0
SCL
NoConn ~ 6100 3100
NoConn ~ 6100 3200
Wire Wire Line
	5650 3500 6100 3500
Wire Wire Line
	5650 3000 6100 3000
$Comp
L Graphic:Logo_Open_Hardware_Small LOGO1
U 1 1 60A0C5E4
P 10150 7100
F 0 "LOGO1" H 10150 7375 50  0001 C CNN
F 1 "Logo_Open_Hardware_Small" H 10150 6875 50  0001 C CNN
F 2 "Symbol:OSHW-Logo2_9.8x8mm_SilkScreen" H 10150 7100 50  0001 C CNN
F 3 "~" H 10150 7100 50  0001 C CNN
	1    10150 7100
	1    0    0    -1  
$EndComp
Text Notes 6400 7150 0    100  ~ 0
Simple Stepper Motor Analyzer MK3
Text Notes 7550 4850 2    50   ~ 0
BACKLIGHT\nCONTROL
Text Notes 8200 6400 0    50   ~ 0
TFT 320x480 ILI9488 16bit data.\n0.5mm pitch FPC, TOP contacts.
Text Notes 5750 4000 0    50   ~ 0
Capacitive touch FT6206 I2C,\n0.5mm pitch FPC, BOTTOM contacts.
Text Notes 4300 7050 2    50   ~ 0
MCU
$Comp
L stepper_motor_analyzer:TestPoint TP3
U 1 1 60B14A2B
P 6650 1200
F 0 "TP3" H 6708 1272 50  0000 L CNN
F 1 "TestPoint" H 6708 1227 50  0001 L CNN
F 2 "stepper_motor_analyzer:testpoint_double_side" H 6850 1200 50  0001 C CNN
F 3 "" H 6850 1200 50  0001 C CNN
	1    6650 1200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 60B15A19
P 6650 1300
F 0 "#PWR03" H 6650 1050 50  0001 C CNN
F 1 "GND" H 6655 1127 50  0000 C CNN
F 2 "" H 6650 1300 50  0001 C CNN
F 3 "" H 6650 1300 50  0001 C CNN
	1    6650 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 1200 6650 1300
Text Notes 6700 1000 2    50   ~ 0
GND
$Comp
L stepper_motor_analyzer:Motor M1
U 1 1 60A13B95
P 1050 2950
F 0 "M1" H 1100 3350 50  0001 L CNN
F 1 "Motor" H 950 3300 50  0001 L TNN
F 2 "stepper_motor_analyzer:null_footprint" H 1060 3040 50  0001 C CNN
F 3 "" H 1060 3040 50  0001 C CNN
	1    1050 2950
	0    -1   -1   0   
$EndComp
Text Notes 8850 1100 0    50   ~ 0
C11097
Text Notes 6050 2800 0    50   ~ 0
C262262
Text Notes 6400 7300 0    50   ~ 0
 LCSC compatible PART# in blue, starts with C.
Text Notes 4200 900  0    50   ~ 0
C49678 
Text Notes 3400 2250 0    50   ~ 0
C49678 
Wire Wire Line
	8900 3750 8700 3750
Connection ~ 8700 3750
Wire Wire Line
	8700 3750 8700 3850
Wire Wire Line
	8900 3850 8700 3850
Connection ~ 8700 3850
Wire Wire Line
	8700 3850 8700 3950
Wire Wire Line
	8900 3950 8700 3950
Connection ~ 8700 3950
Wire Wire Line
	8700 3950 8700 4050
Wire Wire Line
	8900 4050 8700 4050
Connection ~ 8700 4050
Wire Wire Line
	8700 4050 8700 4150
Wire Wire Line
	8900 4150 8700 4150
Connection ~ 8700 4150
Wire Wire Line
	8700 4150 8700 4250
Wire Wire Line
	8900 4250 8700 4250
Connection ~ 8700 4250
Wire Wire Line
	8700 4250 8700 4350
Wire Wire Line
	8900 4350 8700 4350
Connection ~ 8700 4350
Wire Wire Line
	8700 4350 8700 4450
Wire Wire Line
	8900 4450 8700 4450
Connection ~ 8700 4450
Wire Wire Line
	8700 4450 8700 4950
NoConn ~ 4900 6050
NoConn ~ 4900 6150
NoConn ~ 4900 6250
NoConn ~ 4900 6550
NoConn ~ 4900 6650
NoConn ~ 3500 5150
NoConn ~ 4900 5750
$Comp
L Transistor_FET:AO3400A Q1
U 1 1 60D5E604
P 7800 5600
F 0 "Q1" H 8005 5646 50  0000 L CNN
F 1 "AO3400A " H 8005 5555 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 8000 5525 50  0001 L CIN
F 3 "http://www.aosmd.com/pdfs/datasheet/AO3401A.pdf" H 7800 5600 50  0001 L CNN
F 4 "C20917 " H 7800 5600 50  0001 C CNN "LCSC"
	1    7800 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 4550 8500 4550
Connection ~ 8500 4550
Wire Wire Line
	8500 4550 8500 5050
Wire Wire Line
	8150 5200 8150 5300
Wire Wire Line
	8150 5300 7900 5300
Connection ~ 7900 5300
Wire Wire Line
	7900 5300 7900 5400
Wire Wire Line
	7900 5300 7650 5300
Wire Wire Line
	7650 5300 7650 5200
Wire Wire Line
	7900 5800 7900 5850
Wire Wire Line
	7600 5600 7200 5600
Text Label 7200 5600 0    50   ~ 0
TFT_BL
Wire Wire Line
	4900 6350 5150 6350
Connection ~ 5150 6350
Wire Wire Line
	5150 6350 5150 6450
Text Notes 5300 6500 0    50   ~ 0
GP18 LOW:
Text Notes 5300 6400 0    50   ~ 0
GP18 FLOAT:
Text Notes 5850 6500 0    50   ~ 0
MK3
Text Notes 5850 6400 0    50   ~ 0
MK2
Text Notes 5300 6300 0    50   ~ 0
HARDWARE VERSION
Wire Wire Line
	8700 4950 8700 5250
Wire Wire Line
	8900 5050 8500 5050
Connection ~ 8500 5050
Wire Wire Line
	8500 5050 8500 5150
Wire Wire Line
	4900 5350 5650 5350
Text Label 2800 4750 0    50   ~ 0
TFT_SYN
Wire Wire Line
	8900 2050 8050 2050
Text Label 8050 2050 0    50   ~ 0
TFT_SYN
Text Notes 2150 4800 0    50   ~ 0
TFT SYNC IS\nEXPERIMENTAL
Text Notes 7800 4600 0    50   ~ 0
C17398
Wire Wire Line
	3500 6550 2800 6550
Text Label 2800 6550 0    50   ~ 0
CONFIG
$Comp
L stepper_motor_analyzer:SolderJumper_3_Open JP1
U 1 1 60C14AFF
P 900 6550
F 0 "JP1" V 1050 6300 50  0000 L CNN
F 1 "SolderJumper_3_Open" V 855 6617 50  0001 L CNN
F 2 "Jumper:SolderJumper-3_P1.3mm_Open_Pad1.0x1.5mm_NumberLabels" H 900 6550 50  0001 C CNN
F 3 "~" H 900 6550 50  0001 C CNN
	1    900  6550
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 60C1CA9F
P 900 6800
F 0 "#PWR0111" H 900 6550 50  0001 C CNN
F 1 "GND" H 905 6627 50  0000 C CNN
F 2 "" H 900 6800 50  0001 C CNN
F 3 "" H 900 6800 50  0001 C CNN
	1    900  6800
	1    0    0    -1  
$EndComp
$Comp
L stepper_motor_analyzer:+3V3 #PWR0112
U 1 1 60C1CED0
P 900 6300
F 0 "#PWR0112" H 900 6150 50  0001 C CNN
F 1 "+3V3" H 915 6473 50  0000 C CNN
F 2 "" H 900 6300 50  0001 C CNN
F 3 "" H 900 6300 50  0001 C CNN
	1    900  6300
	1    0    0    -1  
$EndComp
Wire Wire Line
	900  6300 900  6350
Wire Wire Line
	900  6750 900  6800
Wire Wire Line
	1050 6550 1350 6550
Text Label 1350 6550 2    50   ~ 0
CONFIG
Text Notes 1400 6650 0    50   ~ 0
RESERVED FOR\nFUTURE\nFEATURES
Text Notes 7450 5800 0    50   ~ 0
C20917
Wire Wire Line
	4300 1500 4750 1500
Wire Wire Line
	3350 2600 4750 2600
$Comp
L Mechanical:MountingHole H4
U 1 1 60A94418
P 5750 7650
F 0 "H4" H 5850 7696 50  0000 L CNN
F 1 "MountingHole" H 5850 7605 50  0001 L CNN
F 2 "MountingHole:MountingHole_2.5mm_Pad_Via" H 5750 7650 50  0001 C CNN
F 3 "~" H 5750 7650 50  0001 C CNN
	1    5750 7650
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 60A9417C
P 5450 7650
F 0 "H3" H 5550 7696 50  0000 L CNN
F 1 "MountingHole" H 5550 7605 50  0001 L CNN
F 2 "MountingHole:MountingHole_2.5mm_Pad_Via" H 5450 7650 50  0001 C CNN
F 3 "~" H 5450 7650 50  0001 C CNN
	1    5450 7650
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 60A93F04
P 5150 7650
F 0 "H2" H 5250 7696 50  0000 L CNN
F 1 "MountingHole" H 5250 7605 50  0001 L CNN
F 2 "MountingHole:MountingHole_2.5mm_Pad_Via" H 5150 7650 50  0001 C CNN
F 3 "~" H 5150 7650 50  0001 C CNN
	1    5150 7650
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H1
U 1 1 60A92EA6
P 4850 7650
F 0 "H1" H 4950 7696 50  0000 L CNN
F 1 "MountingHole" H 4950 7605 50  0001 L CNN
F 2 "MountingHole:MountingHole_2.5mm_Pad_Via" H 4850 7650 50  0001 C CNN
F 3 "~" H 4850 7650 50  0001 C CNN
	1    4850 7650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 4850 2800 4850
Text Label 2800 4850 0    50   ~ 0
~TFT_RST
NoConn ~ 4900 5850
$Comp
L Device:C C3
U 1 1 60C5DA5D
P 5950 1100
F 0 "C3" H 6100 1050 50  0000 L CNN
F 1 "10u" H 6100 1150 50  0000 L CNN
F 2 "stepper_motor_analyzer:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5988 950 50  0001 C CNN
F 3 "~" H 5950 1100 50  0001 C CNN
F 4 "C15850 " V 5950 1100 50  0001 C CNN "LCSC"
	1    5950 1100
	-1   0    0    1   
$EndComp
Text Notes 5550 900  0    50   ~ 0
C15850 
$EndSCHEMATC
