;
; testx.asm
; A 65x02 test program
;

.65x02

; %ps, %ds, %ss, %xs segment registers
.org $40000

entry:
    LDA #$A0
    ADDB D1, 5
    TXB D3, D1

