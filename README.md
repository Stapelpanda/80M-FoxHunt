
## Fuses

	l_fuse = 0xEE
	h_fuse = 0xD9
	e_fuse = 0xFF

## DIP Switches
SW 1 - 3 Vos Nummer
	0	0	0	0	// Home		MOM 	- - 	| - - - 	| - -
	0	0	1	1	// Vos 1 	MOE     - -     | - - -		| .
	0	1	0	2  	// Vos 2 	MOI		- -		| - - -		| . .
	0	1	1	3	// Vos 3 	MOS		- -		| - - -		| . . .
	1	0	0	4	// Vos 4 	MOH		- -		| - - -		| . . . .
	1	0	1	5	// Vos 5 	MO5		- -		| - - -		| . . . . .
	1	1	0	6	// Vos 6 	MO5E	- -		| - - -		| . . . . .	| .
	1	1	1	7	// Vos 7 	MO5I	- -		| - - -		| . . . . . | . .

SW 4 - 5 Schema
 0 - 0: ARDF 		(1 Min,  MaxID = 5, cycle = 5 min)
 0 - 1: ARDF - Fast	(20 Sec,  MaxID = 5, cycle = 1.666 min)
 1 - 0: ARDF7		(1 Min, MaxID = 7, cycle = 7 min)
 1 - 1: ARDF7 - Fast(20 Sec, MaxID = 7, cycle = 2.3333 min)
 
SW 6 - 7 Naming
 0 - 0: ARDF (MOE - MO5)
 0 - 1: ARDF Short (ME - M5)
 1 - 0: PB0MV + Num 
 1 - 1: PI6YRC + Num 
 
SW 8 Debug
  X - For future usage