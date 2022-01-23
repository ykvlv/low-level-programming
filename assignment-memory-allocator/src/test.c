#define _DEFAULT_SOURCE
#include <unistd.h>

#include "mem_internals.h"
#include "mem.h"
#include "util.h"

void debug(const char* fmt, ... );

static void create_new_heap(struct block_header *last_block) {
  struct block_header *addr = last_block;
  void* test_addr = (uint8_t*) addr + size_from_capacity(addr->capacity).bytes;
  test_addr = mmap((uint8_t*) (getpagesize() * ((size_t) test_addr / getpagesize() +
                                                 (((size_t) test_addr % getpagesize()) > 0))), 1000,
                    PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE,0, 0);
  debug(test_addr);
}

static struct block_header* get_block_from_contents(void* data) {
  return (struct block_header *) ((uint8_t *) data - offsetof(struct block_header, contents));
}

void* test_init() {
  void* heap = heap_init(10000);
  if (heap == NULL) {
    err("Куча не была выделена.");
  }
  return heap;
}

void test1(struct block_header* first_block) {
  debug("Тест 1.\n");

  void *data = _malloc(1000);
  if (data == NULL)
    err("Тест провален: malloc не выделил память.");

  debug_heap(stdout, first_block);

  if (first_block->is_free != false || first_block->capacity.bytes != 1000)
    err("Тест провален: is_free или capacity не корректны.");

  debug("Тест 1 прошел успешно.\n\n");
  _free(data);
}

void test2(struct block_header* first_block) {
  debug("Тест 2.\n");

  void *data1 = _malloc(1000);
  void *data2 = _malloc(1000);
  if (data1 == NULL || data2 == NULL)
    err("Тест провален: malloc не выделил память.");

  _free(data1);

  debug_heap(stdout, first_block);

  struct block_header *data1_block = get_block_from_contents(data1);
  struct block_header *data2_block = get_block_from_contents(data2);

  if (data1_block->is_free == false)
    err("Тест провален: блок не был освобожден.");
  if (data2_block->is_free == true)
    err("Тест провален: блок не был освобожден.");

  debug("Тест 2 прошел успешно.\n\n");
  _free(data2);
  _free(data1);
}

void test3(struct block_header* first_block) {
  debug("Тест 3.\n");

  void *data1 = _malloc(1000);
  void *data2 = _malloc(1000);
  void *data3 = _malloc(1000);
  if (data1 == NULL || data2 == NULL || data3 == NULL)
    err("Тест провален: malloc не выделил память.");

  _free(data2);
  _free(data1);

  debug_heap(stdout, first_block);

  struct block_header *data1_block = get_block_from_contents(data1);
  struct block_header *data3_block = get_block_from_contents(data3);
  if (data1_block->is_free == false)
    err("Тест провален: блок не был освобожден.");
  if (data3_block->is_free == true)
    err("Тест провален: блок не был освобожден.");
  if (data1_block->capacity.bytes != 2000 + offsetof(struct block_header, contents))
    err("Тест провален.");

  debug("Тест 3 прошел успешно.\n\n");
  _free(data3);
  _free(data2);
  _free(data1);
}

void test4(struct block_header* first_block) {
  debug("Тест 4.\n");

  void *data1 = _malloc(10000);
  void *data2 = _malloc(10000);
  void *data3 = _malloc(5000);
  if (data1 == NULL || data2 == NULL || data3 == NULL)
    err("Тест провален: malloc не выделил память.");

  _free(data2);
  _free(data3);

  debug_heap(stdout, first_block);

  struct block_header *data1_block = get_block_from_contents(data1);
  struct block_header *data2_block = get_block_from_contents(data2);
  if ((uint8_t *) data1_block->contents + data1_block->capacity.bytes != (uint8_t*) data2_block)
    err("Тест провален.");

  debug("Тест 4 прошел успешно.\n\n");
  _free(data3);
  _free(data2);
  _free(data1);
}

void test5(struct block_header* first_block) {
  debug("Тест 5.\n");
  void *data1 = _malloc(10000);
  if (data1 == NULL)
    err("Тест провален: malloc не выделил память.");

  struct block_header *addr = first_block;
  while (addr->next != NULL) addr = addr->next;
  create_new_heap(addr);
  void *data2 = _malloc(100000);

  debug_heap(stdout, first_block);

  struct block_header *data2_block = get_block_from_contents(data2);
  if (data2_block == addr)
    err("Тест 5 провален.");

  debug("Тест 5 прошел успешно.\n\n");
  _free(data2);
  _free(data1);
}

void test_allocator() {
  void* heap = test_init();
  struct block_header* first_block = (struct block_header*) (heap);
  test1(first_block);
  test2(first_block);
  test3(first_block);
  test4(first_block);
  test5(first_block);
  debug("Все тесты пройдены.\n");
}
