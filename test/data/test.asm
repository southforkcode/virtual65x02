; I am a comment

.module "basic macros"

directive
macro_kw = kw_(".macro", directive)
macro_p = kw_(".macro")

.macro lda imm
    .byte $a9, imm
.endmacro

.macro rts
    .byte $60
.endmacro

start:
.proc main
    lda #1
    rts
.endproc main
