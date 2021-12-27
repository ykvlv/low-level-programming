%include "lib.inc"
%include "dict.inc"

%define buffer_size 255

global _start

section .rodata
%include "colon.inc"
%include "words.inc"
start_text: db "Введите ключ: ", 0x0
found_text: db "Значение: ", 0x0
not_found_text: db "Совпадений не найдено", 0xA, 0x0
too_long_text: db "Значение больше 255 символов", 0xA, 0x0

section .text
_start:
    mov rdi, start_text
    call print_string
    sub rsp, buffer_size ; выделяем место под слово
    mov rdi, rsp
    mov rsi, buffer_size
    call read_word
    cmp rax, 0
    je .too_long ; буфер переполнен
    mov rdi, rsp
    mov rsi, next
    push rdx ; сохраним длину ключа, чтобы потом пропустить его
    call find_word
    add rsp, buffer_size ; освобождаем стек
    cmp rax, 0
    je .not_found ; значение не найдено
    .found:
        add rax, rdx ; адрес начала значения
        push rax
        mov rdi, found_text 
        call print_string ; для начала выведем лейбл
        pop rdi
        call print_string
        call print_newline
        call exit
    .too_long:
        add rsp, buffer_size
        mov rdi, too_long_text
        call print_err_string
        call exit
    .not_found:
        mov rdi, not_found_text
        call print_err_string
        call exit

