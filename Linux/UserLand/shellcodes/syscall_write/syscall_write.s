; Ubuntu 64-bit architecture
section .data
    msg db 'System Calls', 0

section .text
    global main

main:
    ; let's write to stdout
    ; ssize_t write(int fildes, const void *buf, size_t nbyte);
    mov rax, 1          ; syscall is passed in rax register
	
	  ; Function parameters are passed in the registers rdi, rsi, and rdx, respectively.
    mov rdi, 1          ; fildes -> 1 for stdout
    mov rsi, msg        ; buf -> pointer to the message
    mov rdx, 12         ; nbyte -> number of chars in the message
    syscall             ; trigger an interrupt to execute write

    ; syscall to exit
    ; void exit(int status);
    mov rax, 60         ; 60 = 0x3C system call number for exit
	
	  ; One parameter passed in the register rdi
    xor rdi, rdi        ; exit code 0
    syscall             ; trigger an interrupt to execute exit
