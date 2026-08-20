; movcursor.asm

; Params:
;   bh = coord y
;   bl = coord x
;   ah = 02h (function to set cursor position)
;   int 10h (video interrupt)

MovCursor:
    pusha ; save all registers to the stack
    mov dh, bh ; move y coordinate to dh
    mov dl, bl ; move x coordinate to dl
    mov bh, 0 ; set page number to 0
    mov ah, 02h ; function to set cursor position
    int 10h ; call video interrupt
    popa ; restore all registers from the stack
    ret ; return from the procedure