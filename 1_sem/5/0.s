	.intel_syntax noprefix
	.text
	.global calc
	.global vector_sum

calc:
	push ebp
	mov ebp, esp
	sub esp, 8

	movsd xmm0, [ebp + 8]
	addsd xmm0, [ebp + 16]

	cvtsi2sd xmm1, [ebp + 32]
	addsd xmm1, [ebp + 24]
	divsd xmm0, xmm1
	movsd [ebp - 8], xmm0
	fld qword ptr [ebp - 8]

	mov esp, ebp
	pop ebp
	ret

vector_sum:
	push ebp
	mov ebp, esp
	push esi
	push edi
	push ebx

	mov eax, [ebp + 8]
	mov esi, [ebp + 12]
	mov edi, [ebp + 16]
	mov ebx, [ebp + 20]
	mov ecx, 0

	loop:
		movaps xmm0, [esi + ecx * 4]
		movaps xmm1, [edi + ecx * 4]
		addps xmm0, xmm1
		movaps [ebx + ecx * 4], xmm0
		add ecx, 4
		cmp ecx, eax
		jl loop


	pop ebx
	pop edi
	pop esi
	mov esp, ebp
	pop ebp
	ret
