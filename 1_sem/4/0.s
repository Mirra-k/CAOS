	.intel_syntax noprefix
	.text


	.global summ
summ:
	push ebx
	mov ecx, N

	my_loop:
		mov ebx, ecx
		dec ebx
		mov edx, A
		mov eax, [edx + 4*ebx]
		mov edx, B
		add eax, [edx + 4*ebx]
		mov edx, R
		mov [edx + 4*ebx], eax
		loop my_loop
	pop ebx
	ret

	.global everyday795
everyday795:
	push ebp
	mov ebp, esp
	sub esp, 4  // int n; &n = [ebp-4]
	push ebx

	mov ebx, ebp  // ebx = &n
	sub ebx, 4
	push ebx
	mov ebx, offset format_scanf
	push ebx
	call scanf
	add esp, 8

	mov ebx, [ebp-4]    // ebp = n
	imul ebx, [ebp+8]   // n *= x
	add ebx, [ebp+12]   // n += y

	push ebx
	mov ebx, offset format_printf
	push ebx
	call printf
	add esp, 8

	pop ebx
	mov esp, ebp
	pop ebp
	ret


	.data
format_printf:
	.string "%d\n"
format_scanf:
	.string "%d"
