[BITS 64]
extern read_tsc
extern tsc_frequency
extern tsc_microseconds

; 128 bit types not supported by default in C
tsc_microseconds:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    call tsc_frequency
    ; store the TSC frequency at rbp-8
    mov qword [rbp-8], rax
    ; store the current TSC at RBP-16
    call read_tsc
    mov qword [rbp-16], rax

    ; We now have TSC_frequency at rbp-8 and current_tsc at RBP-16
    mov rax, qword [rbp-16]
    mov rcx, 1000000
    ; current_tsc * 1000000
    mul rcx
    ; RDX:RAX = result
    ; RDX for overflow

    ; Divide by the tsc_freq
    ; RDX:RAX / RCX = (current_tsc * 1000000) / tsc_freq
    ; now we do the divide
    mov rcx, qword [rbp-8] ; Holds tsc_frequency
    div rcx
    ; RAX = microseconds

    ; Restore the stack
    add rsp, 32
    pop rbp
    ret

