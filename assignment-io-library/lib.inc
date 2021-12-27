section .text

; Принимает код возврата и завершает текущий процесс
;
; Параметры:    rdi - код возврата
exit:
    mov rax, 60
    syscall


; Принимает указатель на нуль-терминированную строку, возвращает её длину
;
; Параметры:    rdi - указатель на строку
; Возврат:      rax - длина строки
string_length:
    xor rax, rax
    .length:
        cmp byte [rdi+rax], 0x0 ; нуль-терминатор
	je .end ; конец строки
	inc rax
	jmp .length ; продолжить подсчет
    .end:
        ret


; Принимает указатель на нуль-терминированную строку, выводит её в stdout
;
; Параметры:    rdi - указатель на строку
print_string:
    push rdi
    call string_length
    mov rdx, rax
    pop rsi
    mov rdi, 1
    mov rax, 1
    syscall
    ret


; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, 0xA ; перевод строки


; Принимает код символа и выводит его в stdout
;
; Параметры:    rdi - код символа
print_char:
    push rdi
    mov rsi, rsp
    mov rdx, 1
    mov rdi, 1
    mov rax, 1
    syscall
    pop rdi
    ret


; Выводит беззнаковое 8-байтовое число в десятичном формате
;
; Параметры:    rdi - число
print_uint:
    mov rax, rdi
    mov r8, 10 ; делитель
    mov r9, rsp ; сохраним стек, чтобы потом восстановить
    dec rsp
    mov byte [rsp], 0x0 ; нуль-терминатор для правильного вывода строки
    .div:
        xor rdx, rdx
        div r8
        add rdx, 0x30 ; to ASCII
        dec rsp
        mov byte [rsp], dl
        cmp rax, 0
        jne .div ; продолжить деление
    .print:
        mov rdi, rsp
        push r9
        call print_string
        pop rsp
    ret


; Выводит знаковое 8-байтовое число в десятичном формате
;
; Параметры:    rdi - число
print_int:
    cmp rdi, 0
    jge .print ; число неотрицательное
    push rdi
    mov rdi, 0x2D ; minus char
    call print_char
    pop rdi
    neg rdi
    .print:
        jmp print_uint


; Принимает два указателя на нуль-терминированные строки
; Возвращает 1 если они равны, иначе 0
;
; Параметры:    rdi - указатель на первую строку
;               rsi - указатель на вторую строку
; Возврат:      rax - 1 если строки равны, иначе 0
string_equals:
    xor rcx, rcx
    xor r8, r8
    .check:
        mov r8b, byte [rdi+rcx]
        cmp byte [rsi+rcx], r8b
        jne .false
        inc rcx
        cmp r8b, 0x0 ; нуль-терминатор
        jne .check
    .true:
        mov rax, 1
        ret
    .false:
        xor rax, rax
        ret


; Читает один символ из stdin и возвращает его.
; Возвращает 0 если достигнут конец потока
;
; Возврат:      rax - прочитанный символ из потока, иначе 0
read_char:
    mov rax, 0
    mov rdi, 0
    push 0 ; буфер для символа
    mov rsi, rsp
    mov rdx, 1
    syscall
    pop rax
    ret 


; Читает в буфер слово из stdin, пропуская пробельные символы в начале.
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
;
; Параметры:    rdi - указатель на буфер
;               rsi - длина буфера
; Возврат:      rax - адрес буфера, если слово поместилось, иначе 0
;               rdx - длина слова
read_word:
    push rdi
    push rsi
    push 0 ; длина слова
    .skip:
        call read_char
        cmp rax, 0
        je .error_pop ; конец строки
        mov rdi, rax
        call is_whitespace
        cmp rax, 1
        je .skip ; продолжить пропускание
    .read:
        mov rax, rdi
        pop rdx
        pop rsi
        pop rdi
        cmp rdx, rsi
        jge .error ; буфер переполнен
        mov [rdi+rdx], rax
        inc rdx
        push rdi
        push rsi
        push rdx
        call read_char
        cmp rax, 0
        je .end ; конец строки
        mov rdi, rax
        call is_whitespace
        cmp rax, 0
        je .read ; продолжить чтение
    .end:
        pop rdx
        pop rsi
        pop rax
        ret
    .error_pop:
        pop rdx
        pop rsi
        pop rdi
    .error:
        xor rax, rax
        ret 


; Проверяет является ли символ пробельным
; Пробельные символы: 0x20, 0xA, 0x9
;
; Параметры:    rdi - символ
; Возврат:      rax - 1 если символ пробельный, иначе 0
;               rdi - символ
is_whitespace:
    cmp rdi, 0x20 ; символ пробела
    je .true
    cmp rdi, 0x9 ; табуляция
    je .true
    cmp rdi, 0xA ; перевод строки
    je .true
    .false:
        xor rax, rax
        ret
    .true:
        mov rax, 1
        ret


; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
;
; Параметры:    rdi - указатель на строку
; Возврат:      rax - прочитанное число
;               rdx - длина числа в символах, иначе 0
parse_uint:
    xor rdx, rdx
    xor rax, rax
    xor r8, r8
    .parse:
        mov r8b, byte [rdi+rdx]
        cmp r8, 0x0 ; нуль-терминатор
        je .end ; конец числа
        cmp r8, 0x30 ; арабская цифра ноль
        jb .end
        cmp r8, 0x39 ; арабская цифра девять
        ja .end
        sub r8, 0x30 ; to number
        imul rax, 10
        add rax, r8
        inc rdx
        jmp .parse ; продолжить парсинг
    .end:
        ret


; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
;
; Параметры:    rdi - указатель на строку
; Возврат:      rax - прочитанное число
;               rdx - длина числа в символах (включая знак, если он был), иначе 0
parse_int:
    xor rdx, rdx
    xor rax, rax
    cmp byte [rdi], 0x2D ; minus char
    je .negative
    call parse_uint
    ret
    .negative:
        inc rdi
        call parse_uint
        inc rdx
        neg rax
        ret


; Копирует нуль-термированную строку в буфер
;
; Параметры:    rdi - указатель на строку
;               rsi - указатель на буфер
;               rdx - длина буфера
; Возврат:      rax - длина строки если она умещается в буфер, иначе 0
string_copy:
    xor rax, rax
    .copy:
        cmp rax, rdx
	je .error ; буфер переполнен
	mov r11, [rdi+rax]
	mov [rsi+rax], r11
	cmp byte [rdi+rax], 0x0 ; нуль-терминатор
	je .end ; конец строки
	inc rax
	jmp .copy ; продолжить копирование
    .error:
        xor rax, rax
    .end:
        ret

