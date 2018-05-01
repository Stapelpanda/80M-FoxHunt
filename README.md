
## Fuses

	l_fuse = 0xEE
	h_fuse = 0xD9
	e_fuse = 0xFF

## DIP Switches
SW 1 - 3 Vos Nummer
	0	0	0	0	// Thuisvos	MOM 	- - 	| - - - 	| - -
	0	0	1	1	// Vos 1 	MOE     - -     | - - -		| .
	0	1	0	2  	// Vos 2 	MOI		- -		| - - -		| . .
	0	1	1	3	// Vos 3 	MOS		- -		| - - -		| . . .
	1	0	0	4	// Vos 4 	MOH		- -		| - - -		| . . . .
	1	0	1	5	// Vos 5 	MO5		- -		| - - -		| . . . . .
	1	1	0	6	// Vos 6 	MO5E	- -		| - - -		| . . . . .	| .
	1	1	1	7	// Vos 7 	MO5I	- -		| - - -		| . . . . . | . .

SW 4 - 5 Schema
 0 - 0: ARDF 		(1 Min,  MaxID = 5, cycle = 5 min)
 0 - 1: ARDF - 7 	(1 Min,  MaxID = 7, cycle = 15 min)
 1 - 0: FAST		(10 Sec, MaxID = 6, cycle = 1 min)
 1 - 1: Continuous  (Always transmit)
 
SW 6 - 7 Naming
 0 - 0: ARDF (MOE - MO5)
 0 - 1: ARDF Short (ME - M5)
 1 - 0: CAL_A + Num PB0MV
 1 - 1: CAL_B + Num PI6YRC
 
SW 8 Debug
 0 - No Light
 1 - Light