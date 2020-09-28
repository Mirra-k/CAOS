    	.text
   	.global main

main:
	push 	{lr}
	push	{r11}
	mov r11, #0

	loop_read:
		ldr r0, =stdin
		ldr r0, [r0]
		bl fgetc

		cmp r0, #-1
		beq loop_write

		push {r0}
		add r11, r11, #1
		b loop_read

	loop_write:		
		sub r11, r11, #1
		cmp r11, #-1
		beq end

		pop {r0}
		ldr r1, =stdout
		ldr r1, [r1]
		bl fputc
		b loop_write

		
	end:
		pop {r11}
		mov r0, #0
		pop {pc}
