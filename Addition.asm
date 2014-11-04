/*
 * Addition.asm
 *
 *  Created: 10/27/2014 11:57:09 AM
 *   Author: Dariusz Michalski
 */ 
 ;program dodaje do siebie dwie tablice wykorzystujac wskazniki pokazujace na tablice, ktore sa przesownane w kolejnych krokach
 ;po ostatnich zajeciach zmienilem zapis z Big Endian na Little Endian czyli zapusujemy zablice dlugosci 2 np. 00ff + 00ff wynik = 01ffe
.include "m32def.inc" 
 
.DSEG
	.equ LEN=5				;ustawienie dlugosci tablic
	tab_A: .BYTE LEN		;tablica jest zadeklarowane w 'data IRAM' od adresu 0x0060(wg. Atmel Studio) i tam nalezy wprowadzic wartosci
	tab_B: .BYTE LEN		;tablica jest zadeklarowana w 'data IRAM' od adresu 0x0060 + LEN

.CSEG
	LDI XL, LOW(tab_A)		;rejestry X i Y ustawiamy tak aby pokazywaly nam na tablice A i B
	LDI XH, HIGH(tab_A)
	LDI YL, LOW(tab_B)
	LDI YH, HIGH(tab_B)
	LDI ZL, LOW(LEN)		;rejestr Z bedzie potrzebny do zakonczenia dodawania
	LDI ZH, HIGH(LEN)

	change_endian:			;przesowam wskazniki na koniec 
		LD R1, X+
		LD R1, Y+
		SUBI ZL, 1
		BRNE change_endian

	LDI ZL, LOW(LEN)		;przywracamy wartosc rejestru Z
	LDI ZH, HIGH(LEN)

	CP ZL, R1				;rejestry sa ustawione na 0 i dlatego mozemy sprawdzic czy dlugosc
	BRNE start				;tablic nie jest rowna 0 i zakonczyc algorytm
	CP ZH, R1 
	BREQ koniec
	 
	start:
	CLC
	LDI R19, 0				;rejestr pomocniczy potrzebny do dodawania do liczby flagi Carry

	;X i Y wskazuja poza tablicami dlatego wykorzystujemy wlasnosc LD dzieki czemu najpier przesowa wskaznik pozniej zczytuje

	liczenie:
		MOV R20, R18		;po operacji dodawania ADD lub ADC znika flaga Carry bez wzgledu czy bedzie uwzgledniona czy nie
		LD R16, -X			;przesuniecie w lewo i zapis do pamieci tymczasowej
		LD R17, -Y			;przesuniecie w lewo i zapis do pamieci tymczasowej
		ADD R17, R16		;dodawanie
		LDI R18, 0			;zerujemy zapisany stan flagi Carry
		ADC R18, R19		;zapisujemy stan flagi Carry (R19 = 0)
		ADD R17, R20		;dodajemy do liczby przepelnienie jesli wystapilo
		ST Y, R17			;wprowadzamy wynik dodawania
		
		SUBI ZL, 1			;zmniejszam licznik i jezeli dojdzie do 0 to konczymy dodawanie
		BRNE liczenie 

	koniec:
		RJMP koniec			;po zakonczeniu dzialania robiac kolejny ktok "Step Into" program zaczal by prace od poczatku wiec go zatrzymujemy w tym miejscu
