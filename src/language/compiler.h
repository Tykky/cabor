#pragma once

#include "ir.h"
#include "codegen.h"

static const char* cabor_preamble =
".global _start\n"
".global print_int\n"
".global print_bool\n"
".global read_int\n"
".extern main\n"
".section .text\n\n"

"# BEGIN START (we skip this part when linking with C)\n"
"# ***** Function '_start' *****\n"
"# Calls function 'main' and halts the program\n\n"
"_start:\n"
"    call main\n"
"    movq $60, %rax\n"
"    xorq %rdi, %rdi\n"
"    syscall\n"
"# END START\n\n"

"# ***** Function 'print_int' *****\n"
"# Prints a 64-bit signed integer followed by a newline.\n"
"print_int:\n"
"    pushq %rbp\n"
"    movq %rsp, %rbp\n"
"    movq %rdi, %r10\n"
"    decq %rsp\n"
"    movb $10, (%rsp)\n"
"    decq %rsp\n"
"    xorq %r9, %r9\n"
"    xorq %rax, %rax\n"
"    cmpq $0, %rdi\n"
"    je .Ljust_zero\n"
"    jge .Ldigit_loop\n"
"    incq %r9\n"
".Ldigit_loop:\n"
"    cmpq $0, %rdi\n"
"    je .Ldigits_done\n"
"    movq %rdi, %rax\n"
"    movq $10, %rcx\n"
"    cqto\n"
"    idivq %rcx\n"
"    movq %rax, %rdi\n"
"    cmpq $0, %rdx\n"
"    jge .Lnot_negative\n"
"    negq %rdx\n"
".Lnot_negative:\n"
"    addq $48, %rdx\n"
"    movb %dl, (%rsp)\n"
"    decq %rsp\n"
"    jmp .Ldigit_loop\n"
".Ljust_zero:\n"
"    movb $48, (%rsp)\n"
"    decq %rsp\n"
".Ldigits_done:\n"
"    cmpq $0, %r9\n"
"    je .Lminus_done\n"
"    movb $45, (%rsp)\n"
"    decq %rsp\n"
".Lminus_done:\n"
"    movq $1, %rax\n"
"    movq $1, %rdi\n"
"    movq %rsp, %rsi\n"
"    incq %rsi\n"
"    movq %rbp, %rdx\n"
"    subq %rsp, %rdx\n"
"    decq %rdx\n"
"    syscall\n"
"    movq %rbp, %rsp\n"
"    popq %rbp\n"
"    movq %r10, %rax\n"
"    ret\n\n"

"# ***** Function 'print_bool' *****\n"
"print_bool:\n"
"    pushq %rbp\n"
"    movq %rsp, %rbp\n"
"    movq %rdi, %r10\n"
"    cmpq $0, %rdi\n"
"    jne .Ltrue\n"
"    movq $false_str, %rsi\n"
"    movq $false_str_len, %rdx\n"
"    jmp .Lwrite\n"
".Ltrue:\n"
"    movq $true_str, %rsi\n"
"    movq $true_str_len, %rdx\n"
".Lwrite:\n"
"    movq $1, %rax\n"
"    movq $1, %rdi\n"
"    syscall\n"
"    movq %rbp, %rsp\n"
"    popq %rbp\n"
"    movq %r10, %rax\n"
"    ret\n\n"

"true_str:\n"
"    .ascii \"true\\n\"\n"
"true_str_len = . - true_str\n"
"false_str:\n"
"    .ascii \"false\\n\"\n"
"false_str_len = . - false_str\n\n"

"# ***** Function 'read_int' *****\n"
"read_int:\n"
"    pushq %rbp\n"
"    movq %rsp, %rbp\n"
"    pushq %r12\n"
"    pushq $0\n"
"    xorq %r9, %r9\n"
"    xorq %r10, %r10\n"
"    xorq %r12, %r12\n"
".Lloop:\n"
"    xorq %rax, %rax\n"
"    xorq %rdi, %rdi\n"
"    movq %rsp, %rsi\n"
"    movq $1, %rdx\n"
"    syscall\n"
"    cmpq $0, %rax\n"
"    jg .Lno_error\n"
"    je .Lend_of_input\n"
"    jmp .Lerror\n"
".Lend_of_input:\n"
"    cmpq $0, %r12\n"
"    je .Lerror\n"
"    jmp .Lend\n"
".Lno_error:\n"
"    incq %r12\n"
"    movq (%rsp), %r8\n"
"    cmpq $10, %r8\n"
"    je .Lend\n"
"    cmpq $45, %r8\n"
"    jne .Lnegation_done\n"
"    xorq $1, %r9\n"
".Lnegation_done:\n"
"    cmpq $48, %r8\n"
"    jl .Lloop\n"
"    cmpq $57, %r8\n"
"    jg .Lloop\n"
"    subq $48, %r8\n"
"    imulq $10, %r10\n"
"    addq %r8, %r10\n"
"    jmp .Lloop\n"
".Lend:\n"
"    cmpq $0, %r9\n"
"    je .Lfinal_negation_done\n"
"    neg %r10\n"
".Lfinal_negation_done:\n"
"    popq %r12\n"
"    movq %rbp, %rsp\n"
"    popq %rbp\n"
"    movq %r10, %rax\n"
"    ret\n"
".Lerror:\n"
"    movq $1, %rax\n"
"    movq $2, %rdi\n"
"    movq $read_int_error_str, %rsi\n"
"    movq $read_int_error_str_len, %rdx\n"
"    syscall\n"
"    movq $60, %rax\n"
"    movq $1, %rdi\n"
"    syscall\n"

"read_int_error_str:\n"
"    .ascii \"Error: read_int() failed to read input\\n\"\n"
"read_int_error_str_len = . - read_int_error_str\n";


cabor_x64_assembly* cabor_compile(const char* code, const char* filename);
void cabor_write_asmbl_to_file(const char* filename, cabor_x64_assembly* asmbl);

