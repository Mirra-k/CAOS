	.text
	.global sort_by_age

sort_by_age:

	start_sort:
		push {r4, r5, r6, r7, r8, r9}

		mov r9, #48	// смещение для структур
		mov r2, #0 	// счетчик для 1-го цикла

	first_loop:
		mov r3, #0	// счетчик для 2-го цикла

		add r2, r2, #1
		cmp r2, r0
		blt second_loop

		b end

	second_loop:
		mul r4, r3, r9
		add r4, r4, #4
		add r5, r4, r9

		ldrb r6, [r1, r4]	// age 1st person
		ldrb r7, [r1, r5]	// age 2st person

		sub r4, r4, #4
		sub r5, r5, #4

		mov  r8, #0
		cmp r6, r7
		bgt swap

		add r3, r3, #1
		cmp r3, r2
		beq first_loop

		b second_loop

	swap:
		ldr r6, [r1, r4]
		ldr r7, [r1, r5]
		str r6, [r1, r5]
		str r7, [r1, r4]

		add r4, r4, #4
		add r5, r5, #4

		add r8, r8, #1
		cmp r8, #12
		beq second_loop

		b swap
		
	end:
		pop {r4, r5, r6, r7, r8, r9}
		bx lr
