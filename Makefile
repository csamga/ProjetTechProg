CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c89 -O0 -ggdb3

src_dir = src
tmp_dir = tmp
bin_dir = bin

src = $(wildcard $(src_dir)/*.c)
obj = $(patsubst $(src_dir)/%.c,$(tmp_dir)/%.o,$(src))
projet = $(bin_dir)/projet

$(projet): $(obj)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^

$(tmp_dir)/%.o: $(src_dir)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: run
run: $(projet)
	./$(projet)

.PHONY: clean
clean:
	-$(RM) $(projet) $(filter-out %.c,$(obj))
