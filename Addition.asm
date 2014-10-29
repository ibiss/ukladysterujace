/*
 * Addition.asm
 *
 *  Created: 10/27/2014 11:57:09 AM
 *   Author: Dariusz Michalski
 */ 
 ;program dodaje do siebie dwie tablice wykorzystujac wskazniki pokazujace na tablice, ktore sa przesownane w kolejnych krokach
 
.include "m32def.inc" 
 
.DSEG
	.equ LEN=2				;ustawienie dlugosci tablic
	tab_A: .BYTE LEN		;tablica jest zadeklarowane w 'data IRAM' od adresu 0x0060(wg. Atmel Studio) i tam nalezy wprowadzic wartosci
	tab_B: .BYTE LEN		;tablica jest zadeklarowana w 'data IRAM' od adresu 0x0060 + LEN

.CSEG
	LDI XL, LOW(tab_A)		;rejestry X i Y ustawiamy tak aby pokazywaly nam na tablice A i B
	LDI XH, HIGH(tab_A)
	LDI YL, LOW(tab_B)
	LDI YH, HIGH(tab_B)
	LDI ZL, LOW(LEN)		;rejestr Z bedzie potrzebny do zakonczenia dodawania
	LDI ZH, HIGH(LEN)

	CP ZL, R1				;rejestry sa ustawione na 0 i dlatego mozemy sprawdzic czy dlugosc
	BRNE start				;tablic nie jest rowna 0 i zakonczyc algorytm
	CP ZH, R1 
	BREQ koniec
	 
	start:
	CLC

	liczenie:
		LD R1, X+			;ladujemy do R1 wartosc z bitu jaki pokazuje X, czyi na poczatku tab_A[0], nastepnie przesuniemy wskaznik na tab_A[1]
		LD R2, Y			;ladujemy do R2, ale nie przesowamy, poniewaz do tab_B[0] wprowadzimy wynik dodawania tab_A[0]+ tab_B[0]
		ADC R2, R1
		ST Y+, R2			;wprowadzamy wynik dodawania i przesowamy wskaznik pokazujacy na tab_B[1]
		SBIW ZL, 1			;zmniejszam licznik i jezeli dojdzie do 0 to konczymy dodawanie
		BRNE liczenie 

	koniec:
		RJMP koniec			;po zakonczeniu dzialania robiac kolejny ktok "Step Into" program zaczal by prace od poczatku wiec go zatrzymujemy w tym miejscu