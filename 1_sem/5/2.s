	.intel_syntax noprefix
	.text
	.global dot_product

dot_product:
	push ebp
	mov  ebp, esp
	mov eax, [ebp + 8]  // N

	loop:
		cmp eax, 4
		jb end

		sub eax, 4
		mov ebx, [ebp + 12] // A
		movups xmm0, [ebx + 4  * eax]
		mov ebx, [ebp + 16]  // B
		movups xmm1, [ebx + 4 * eax]
		mulps xmm0, xmm1  // A*B
		haddps xmm0, xmm0 // x3+x4 || x0+x1
		haddps xmm0, xmm0 // x3+x4+x0+x1
		addss xmm2, xmm0
		jmp loop

	end:
		cmp eax, 0
		je exit

		sub eax, 1
		mov ebx, [ebp + 12]
		movss xmm0, [ebx + 4 * eax]
		mov ebx, [ebp + 16]
		movss xmm1, [ebx + 4 * eax]
		mulss xmm0, xmm1
		addss xmm2, xmm0
		jmp end

	exit:
		sub esp, 4
		movss [esp], xmm2
		fld dword ptr [esp]
		add esp, 4
		pop ebp
		ret
