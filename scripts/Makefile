target=parse
.PHONY: all $(target) $(OUT)/%.o
all: $(target)

INCLUDE := -I./include/file -I./include/debug/ -I./include/lib
SRCS = debug/dbg.c
SRCS += parse_boot_img/main.c
OBJS = $(patsubst %.c, $(OUT)/%.o, $(wildcard $(SRCS)))

$(target): $(OBJS)
	$(CC) $(OBJS) -o $(OUT)/$@

$(OUT)/%.o: %.c
	[ -e $@ ] || mkdir -p $(dir $@); \
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@
