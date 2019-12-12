;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
     .cdecls C,LIST,"msp430g2553.h"       ; Include device header file

;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.
LED1 		.set	BIT7

DelayLoops	.set	27000
BUZZER			.set	2
SPACE		.set	2
LETTER		.set	0
ENDTX		.set	0xFF
Message		.byte	BUZZER, ENDTX

;--------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------

main:
	;LED
	bis.b	#LED1, &P1OUT
	bis.b	#LED1, &P1DIR
	clr.w	R5
	clr.w	R7


	jmp		MessageTest

MessageLoop:
	bic.b	#LED1,&P1OUT
	mov.b	Message(R5),R12
	call	#DelayTenths
	bis.b	#LED1,&P1OUT
	mov.w	#SPACE,R12
    call	#DelayTenths
    inc.w	R5

MessageTest:
	cmp.b	#ENDTX,Message(R5)
	jne 	MessageLoop

InfLoop:
	mov.w	#0x00, &P1OUT
	jne	 	InfLoop		; IF r7=0 JUMP RESET ; else Infloop;
	jmp		main

DelayTenths:
	jmp		LoopTest

OuterLoop:
	mov.w	#DelayLoops,R4

DelayLoop:
	dec.w	R4
	jnz		DelayLoop
	dec.w	R12

LoopTest:
	cmp.w	#0,R12
	jnz		OuterLoop
	ret

;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack

;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET



