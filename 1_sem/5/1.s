	.intel_syntax noprefix
	.text
	.global my_sin

// 0 - summ((-1)^2 * x^(2n+1) / (2n+1)!)
// 1 - (-1)^2 * x^(2n+1) / (2n+1)!
// 2 - (-1)^2 * x^(2n+1)
// 3 - (2n+1)!
// 4 - x
// 5 - -1
// 6 - 0
// eax - n

my_sin:
	push	ebp
	mov	ebp, esp

	movsd xmm0, [ebp + 8]	
	movsd xmm2, xmm0
	mov eax, 1
	cvtsi2sd xmm3, eax	
	movsd xmm4, xmm0	
	mov ecx, -1		
	cvtsi2sd xmm5, ecx	
	mov ecx, 0
	cvtsi2sd xmm6, ecx	

	.loop:
		mov ecx, eax		// n
		add ecx, ecx		// 2n
		cvtsi2sd xmm7, ecx
		mulsd xmm3, xmm7	// 2n!
		inc ecx			// 2n + 1
		cvtsi2sd xmm7, ecx
		mulsd   xmm3, xmm7	// (2n + 1)!

		mulsd xmm2, xmm5	// k*(-1)
		mulsd xmm2, xmm4	// k*x
		mulsd xmm2, xmm4	// k*x^2

		movsd   xmm1, xmm2
		divsd   xmm1, xmm3
		addsd   xmm0, xmm1
		inc eax
	
		comisd  xmm1, xmm6
		jz  .finalize
		jmp .loop

	.finalize:
		sub esp, 8
		movsd [esp], xmm0
		fld qword ptr [esp]
		add esp, 8
		leave
		ret
