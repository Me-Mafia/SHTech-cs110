# see <https://makefiletutorial.com/> for a comprehensive & comprehendible tutorial on GNU make

CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wpedantic
CFLAGS_DYNAMIC = -shared

test_src = tester/file_tester.c tester/dir_tester.c tester/explorer_tester.c

src_files = file.c dir.c node.c explorer.c
obj_files = file.o dir.o node.o explorer.o
static_obj_files  = file.o-static  dir.o-static  node.o-static  explorer.o-static
dynamic_obj_files = file.o-dynamic dir.o-dynamic node.o-dynamic explorer.o-dynamic
static_lib  = libexplorer.a
dynamic_lib = libexplorer.so


all: $(obj_files) $(static_obj_files) $(dynamic_obj_files)
.PHONY: all 

%.o : %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $^ -o $@
%.o-static : %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $^ -o $@
%.o-dynamic : %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -fPIC $^ -o $@

clean:
	rm -f $(obj_files)
	rm -f $(static_obj_files) $(static_lib)
	rm -f $(dynamic_obj_files) $(dynamic_lib)
	rm -f test.out test.static.out test.dynamic.out

# the statically linked library (or archive library)
$(static_lib): $(static_obj_files)
	ar crv $@ $^

# the dynamically linked library (or shared object)
$(dynamic_lib): $(dynamic_obj_files)
	$(CC) -shared -fPIC -o $@ $^

test.out:         main.c $(test_src) $(obj_files)
	$(CC) $(CFLAGS) $^ -o $@
test.static.out:  main.c $(test_src) $(static_lib)
	$(CC) -static $(CFLAGS) $^ -L./ -o $@ 
test.dynamic.out: main.c $(test_src) $(dynamic_lib)
	$(CC) -fPIC $(CFLAGS) $^ -o $@ -Wl,-rpath=.