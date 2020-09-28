	.intel_syntax noprefix
	.text
	.global _start

_start:
	push ebp
	mov ebp, esp

.loop:
	sub esp, 4
	mov eax, 3
	mov ebx, 0
	mov ecx, esp
	mov edx, 1
	int 0x80
	
	cmp eax, 0
	je .end

	mov eax, 4
	mov ebx, 1
	mov ecx, esp
	mov edx, 1
	int 0x80

	add esp, 4
	jmp .loop

.end:
	add esp, 4	
	leave
	mov eax, 1
	mov ebx, 1
	int 0x80

