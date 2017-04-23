; Print.s
; Student names: Tiffany Jeong & Isabelle Villamiel
; Last modification date: 4/23/2017
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB
TEN RN R1 
  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
n   EQU 4   ; binding
R   EQU 0
	
    PUSH {R12, LR}
    SUB SP, #8
Here
    CMP R0, #10
    BLO LESS
    
    MOV TEN, #10
    UDIV R2, R0, TEN
    MUL R2, TEN
    SUB R2, R0, R2
    STR R2, [SP, #R]
    UDIV R0, TEN
    STR R0, [SP, #n]
    BL  LCD_OutDec
    LDR R0, [SP, #n]
    BL  Here
    ADD SP, #8
    POP {R12, LR}
    BX  LR
LESS
    ADD R0, #0x30
    BL  ST7735_OutChar
    ADD SP, #8
    POP {R12, LR}
    LDR R0, [SP, #R]
    STR R0, [SP, #n]
    

      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
m EQU 0 ; binding

    PUSH {R0, LR}
    STR R0, [SP, #m]    ; m = n
    MOV R1, #1000
    UDIV R2, R0, R1     ; R2 gets n / 1000
    CMP R2, #9
    BHI Stars           ; if n > 9999, print stars
    
    MOV R0, R2          
    ADD R0, #0x30       ; add ASCII offset
    PUSH {R1, R2}
    BL ST7735_OutChar   ; print m/1000
    POP {R1, R2}
    
    MOV R0, #0x2E
    PUSH {R1, R2}
    BL ST7735_OutChar   ; print "."
    POP {R1, R2}
    
    LDR R0, [SP, #m]
    MUL R2, R1
    SUB R0, R0, R2
    STR R0, [SP, #m]    ; m = n % 1000
    
    MOV R1, #100
    UDIV R2, R0, R1     ; R2 gets m / 100
    
    MOV R0, R2
    ADD R0, #0x30       ; add ASCII offset
    PUSH {R1, R2}
    BL  ST7735_OutChar  ; print m / 100
    POP {R1, R2}

    LDR R0, [SP, #m]
    MUL R2, R1
    SUB R0, R2
    STR R0, [SP, #m]    ; m = m % 100
    
    MOV R1, #10
    UDIV R2, R0, R1     ; R2 gets m / 10
    
    MOV R0, R2
    ADD R0, #0x30
    PUSH {R1, R2}
    BL ST7735_OutChar   ; print m / 10
    POP {R1, R2}
    
    LDR R0, [SP, #m]
    MUL R2, R1
    SUB R0, R2
    STR R0, [SP, #m]    ; m = m % 10


    ADD R0, #0x30
    PUSH {R1, R2}
    BL ST7735_OutChar   ; print m
    POP {R1, R2}

    POP {R0, LR}
    BX   LR
Stars
    MOV R0, #0x2A       ; print '*'
    BL ST7735_OutChar
    
    MOV R0, #0x2E       ; print '.'
    BL ST7735_OutChar
    
    MOV R0, #0x2A       ; print '*'
    BL ST7735_OutChar
    
    MOV R0, #0x2A       ; print '*'
    BL ST7735_OutChar
    
    MOV R0, #0x2A       ; print '*'
    BL ST7735_OutChar
    
    POP {R0, LR}
    BX LR
 
     BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
