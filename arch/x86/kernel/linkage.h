#ifndef _LINKAGE_H_
#define _LINKAGE_H_

#define SYMBOL_NAME(x) x

#define SYMBOL_NAME_LABEL(x) x##:

#define ENTRY(name)	\
	.global SYMBOL_NAME(name); \
	SYMBOL_NAME_LABEL(name)

#endif
