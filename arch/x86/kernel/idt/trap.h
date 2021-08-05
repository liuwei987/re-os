#ifndef __TRAP_H__
#define __TRAP_H__

#include "lib.h"
#include "printk.h"

void sys_vector_init();
void divide_error();
void nmi();
void invalid_TSS();
void page_fault();

#endif
