NAME       := ladlib
VERSION    := 1.0.0
SOVERSION  := 1

PREFIX     ?= /usr/local
LIBDIR     ?= $(PREFIX)/lib
INCDIR     ?= $(PREFIX)/include

CC         ?= cc
CFLAGS     ?= -O2 -g
override CFLAGS += -std=c11 -Wall -Wextra -Wpedantic -fPIC -MMD -MP -Iinclude

SRC_DIR    := src
BUILD_DIR  := build
OBJ_DIR    := $(BUILD_DIR)/obj
HDR_DIR    := include/$(NAME)

SRCS       := $(shell find $(SRC_DIR) -name '*.c')
OBJS       := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS       := $(OBJS:.o=.d)

LIB_FULL   := lib$(NAME).so.$(VERSION)
LIB_SONAME := lib$(NAME).so.$(SOVERSION)
LIB_LINK   := lib$(NAME).so

TESTS      := $(wildcard main.c test_*.c)
TEST_BINS  := $(TESTS:%.c=$(BUILD_DIR)/%)

.PHONY: all clean install uninstall test

all: $(BUILD_DIR)/$(LIB_FULL)

$(BUILD_DIR)/$(LIB_FULL): $(OBJS)
	$(CC) -shared -Wl,-soname,$(LIB_SONAME) $(OBJS) -o $@
	ln -sf $(LIB_FULL) $(BUILD_DIR)/$(LIB_SONAME)
	ln -sf $(LIB_SONAME) $(BUILD_DIR)/$(LIB_LINK)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	install -d $(DESTDIR)$(LIBDIR) $(DESTDIR)$(INCDIR)
	install -m 755 $(BUILD_DIR)/$(LIB_FULL) $(DESTDIR)$(LIBDIR)/$(LIB_FULL)
	ln -sf $(LIB_FULL) $(DESTDIR)$(LIBDIR)/$(LIB_SONAME)
	ln -sf $(LIB_SONAME) $(DESTDIR)$(LIBDIR)/$(LIB_LINK)
	rm -rf $(DESTDIR)$(INCDIR)/$(NAME)
	cp -r $(HDR_DIR) $(DESTDIR)$(INCDIR)/$(NAME)
	chmod -R a+rX $(DESTDIR)$(INCDIR)/$(NAME)
ifeq ($(DESTDIR),)
	-ldconfig
endif
	@echo "Installed to $(LIBDIR) and $(INCDIR)/$(NAME)"

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/$(LIB_FULL) $(DESTDIR)$(LIBDIR)/$(LIB_SONAME) $(DESTDIR)$(LIBDIR)/$(LIB_LINK)
	rm -rf $(DESTDIR)$(INCDIR)/$(NAME)
ifeq ($(DESTDIR),)
	-ldconfig
endif

test: $(TEST_BINS)
	@for t in $(TEST_BINS); do echo "== $$t"; ./$$t || exit 1; done

$(BUILD_DIR)/%: %.c $(SRCS)
	@mkdir -p $(BUILD_DIR)
	$(CC) -std=c11 -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined -I. -Iinclude $< $(SRCS) -o $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)