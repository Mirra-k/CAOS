	.text
	.global solve

solve:

	push {r4, r5, r6}
	mov r4, #0   	/* r4 -> значение переменной */
			/* r6 -> ответ */

	loop:

	/* r5 -> Ax3 */
	mov r5, r0
	mul r5, r5, r4
	mul r5, r5, r4
	mul r5, r5, r4
	
	/* r6 -> Ax3 */
	mov r6, r5

	/* r5 -> Bx2 */
	mov r5, r1
	mul r5, r5, r4
	mul r5, r5, r4

	/* r6 -> Ax3 + Bx2 */
	add r6, r6, r5

	/* r5 -> Cx */
	mov r5, r2
	mul r5, r5, r4
	add r6, r6, r5

	/* r6 -> Ax3 + Bx2 + Cx */
	add r6, r6, r3

	/* r6 -> Ax3 + Bx2 + Cx + D */
	add r4, r4, #1

	cmp r6, #0
	bne loop

	mov r0, r4
	sub r0, r0, #1
	pop {r4, r5, r6}
	bx lr
