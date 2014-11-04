/*
 * Addition.asm
 *
 *  Created: 10/27/2014 11:57:09 AM
 *   Author: Dariusz Michalski
 */ 

 ;po ostatnich zajeciach zmienilem zapis z Big Endian na Little Endian czyli zapusujemy zablice dlugosci 2 np. 00ff + 00ff wynik = 01ffe
.include "m32def.inc" 
 
.DSEG
	.equ LEN=5				;ustawienie dlugosci tablic
	tab_A: .BYTE LEN
	tab_B: .BYTE LEN
	
.CSEG 
    LDI R16, low(RAMEND)    ;deklaracja stosu 
    LDI R17, high(RAMEND) 
    out SPL, R16 
    out SPH, R17 
 
    LDI XL, LOW(tab_A) 
    LDI XH, HIGH(tab_A) 
    LDI YL, LOW(tab_B) 
    LDI YH, HIGH(tab_B) 
    LDI ZL, LOW(LEN) 
    LDI ZH, HIGH(LEN) 

	change_endian:			;przesowam wskazniki na koniec 
		LD R1, X+
		LD R1, Y+
		SUBI ZL, 1
		BRNE change_endian

	LDI ZL, LOW(LEN)		;przywracamy wartosc rejestru Z
	LDI ZH, HIGH(LEN)

    call DODAJ 
 
    START: 
    JMP START 

/* 
* Parametry: 
*  X - Adres za pierwsza liczba 
*  Y - Adres za druga liczba
*  Z - Dlugosc liczby 
*/ 
     
DODAJ:
	;na stos wrzucamy stany rejestrow, aby po wykonaniu procedury byl spojny
    push R1
    push R2
	push R18
	push R19
	push R20
	LDI R19, 0				;rejestr pomocniczy potrzebny do dodawania do liczby flagi Carry
    liczenie: 
		MOV R20, R18		;po operacji dodawania ADD lub ADC znika flaga Carry bez wzgledu czy bedzie uwzgledniona czy nie
		LD R1, -X			;przesuniecie w lewo i zapis do pamieci tymczasowej
		LD R2, -Y			;przesuniecie w lewo i zapis do pamieci tymczasowej
		ADD R2, R1			;dodawanie
		LDI R18, 0			;zerujemy zapisany stan flagi Carry
		ADC R18, R19		;zapisujemy stan flagi Carry (R19 = 0)
		ADD R2, R20			;dodajemy do liczby przepelnienie jesli wystapilo
		ST Y, R2			;wprowadzamy wynik dodawania
        SBIW ZL, 1 
        BRNE liczenie 
    pop R20
	pop R19
	pop R18
	pop R2 
    pop R1 
    ret
