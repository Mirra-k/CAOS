	.intel_syntax noprefix
	.text
	.global _start

_start:
	mov eax, 4
	mov ebx, 1
	mov ecx, hello_ptr
	mov edx, 13
	int 0x80

	mov eax, 1
	mov ebx, [esp + 4]
	int 0x80

	.data
hello:
	.string "Hello, World!"
hello_ptr:
	.long hello
