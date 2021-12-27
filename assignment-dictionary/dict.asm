%include "lib.inc" ; можно и "extern string_equals", но мне так больше нравится(

%define size 8

global find_word

section .text
; Пройдёт по всему словарю в поисках подходящего ключа.
; Если подходящее вхождение найдено, вернёт адрес начала вхождения, иначе вернёт 0
;
; Параметры:    rdi - указатель на нуль-терминированную строку
;               rsi - указатель на начало словаря
; Возврат:      rax - адрес начала вхождения в словарь, если найдено, иначе 0
find_word:
.find:
    add rsi, size ; указатель на следующий элемент
    push rdi
    push rsi
    call string_equals
    pop rsi
    pop rdi
    cmp rax, 1
    je .found ; ключ найден
    mov rsi, [rsi-size] ; указатель на следующий элемент
    cmp rsi, 0
    jne .find ; продолжить поиск
.end:
    xor rax, rax
    ret
.found:
    mov rax, rsi
    ret      

