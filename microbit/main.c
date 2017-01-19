#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/stackctrl.h"

#include "lib/utils/pyexec.h"
#include "lib/utils/interrupt_char.h"

#include "serial_api.h"
#include "us_ticker_api.h"

serial_t s;

extern int __end__;
extern uint32_t  __HeapLimit;

caddr_t _sbrk(int incr) {
    static unsigned char* heap = (unsigned char*)&__end__;
    unsigned char* prev_heap = heap;
    unsigned char* new_heap = heap + incr;

    if (new_heap >= (unsigned char*)&__HeapLimit) {     /* __HeapLimit is end of heap section */
        return (caddr_t)-1;
    }

    heap = new_heap;
    return (caddr_t) prev_heap;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        return serial_getc(&s);
    }
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    for (; len > 0; --len) {
        serial_putc(&s, *str++);
    }
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    for (; len > 0; --len) {
        if (*str == '\n') {
            serial_putc(&s, '\r');
        }
        serial_putc(&s, *str++);
    }
}

mp_uint_t mp_hal_ticks_ms(void) {
    return us_ticker_read() / 1000;
}

mp_uint_t mp_hal_ticks_us(void) {
    return us_ticker_read();
}

void mp_hal_delay_ms(mp_uint_t ms) {
    mp_uint_t f = mp_hal_ticks_ms() + ms;
    while (mp_hal_ticks_ms() < f) {
    }
}

void mp_hal_delay_us(mp_uint_t us) {
    mp_uint_t f = mp_hal_ticks_us() + us;
    while (mp_hal_ticks_us() < f) {
    }
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return mp_hal_ticks_us() * 1000;
}

void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
    if (lex == NULL) {
        return;
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}

static char *stack_top;

void mp_run(void) {
    int stack_dummy;
    stack_top = (char*)&stack_dummy;
    mp_stack_ctrl_init();
    mp_stack_set_limit(1800);

    // allocate the heap statically in the bss
    static uint8_t heap[10240];
    gc_init(heap, heap + sizeof(heap));

    mp_init();
    do_str("print('hello world!', list(x+1 for x in range(10)), end='eol\\n')", MP_PARSE_SINGLE_INPUT);
    do_str("for i in range(10):\n  print(i)", MP_PARSE_FILE_INPUT);

    for (;;) {
        if (pyexec_friendly_repl() != 0) {
            break;
        }
    }

    mp_deinit();
}

int main(int argc, char **argv) {
    serial_init(&s, USBTX, USBRX);
    serial_baud(&s, 115200);
    char msg[] = "micro:bit!\n";
    mp_hal_stdout_tx_strn_cooked(msg, sizeof(msg));
    mp_hal_set_interrupt_char(3);

    while (1) {
        mp_run();
    }

    return 0;
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    return NULL;
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
}

void NORETURN __fatal_error(const char *msg) {
    while (1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif

void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
}
