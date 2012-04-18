
APP	 = itpu

SRCEXT   = c
SRCDIR   = src
OBJDIR   = obj
BINDIR   = build

CC = gcc
SRCS    := $(shell find $(SRCDIR) -name '*.$(SRCEXT)')
SRCDIRS := $(shell find . -name '*.$(SRCEXT)' -exec dirname {} \; | uniq)
OBJS    := $(patsubst %.$(SRCEXT),$(OBJDIR)/%.o,$(SRCS))

DEBUG    = -g
INCLUDES =
# CFLAGS   = -Wall -pedantic -c $(DEBUG) $(INCLUDES)
CFLAGS   = -Wall -c $(DEBUG) $(INCLUDES)
LDFLAGS  = -lncurses

# ifeq ($(SRCEXT), cpp)
# CC       = $(CXX)
# else
# CFLAGS  += -std=gnu99
# endif

.PHONY: all clean distclean

all: $(BINDIR)/$(APP)

$(BINDIR)/$(APP): buildrepo $(OBJS)
	@mkdir -p `dirname $@`
	@echo "Linking $@..."
	@echo $(CC) $(OBJS) $(LDFLAGS) -o $@
	@$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: %.$(SRCEXT)
	@echo "Generating dependencies for $<..."
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@echo "Compiling $<..."
	@echo $(CC) $(CFLAGS) $< -o $@
	@$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) -r $(OBJDIR)

distclean: clean
	$(RM) -r $(BINDIR)

buildrepo:
	@$(call make-repo)

define make-repo
   for dir in $(SRCDIRS); \
   do \
	mkdir -p $(OBJDIR)/$$dir; \
   done
endef


# usage: $(call make-depend,source-file,object-file,depend-file)
define make-depend
  $(CC) -MM       \
        -MF $3    \
        -MP       \
        -MT $2    \
        $(CFLAGS) \
        $1
endef

# 
# BIN = itpu
# SRC = $(OBJ:%.o=%.c)
# INCLUDES =
# 
# CFLAGS = -Wall -g
# LDFLAGS = -lncurses
# 
# $(BIN): $(SRC)
#       gcc $(CFLAGS) -o $(BIN) $(SRC) $(LDFLAGS)
# 
# %.o: %.c
#         $(CC) $(CFLAGS) -c $<
#       
# .PHONY: clean
# clean:
#       rm -rf $(BIN) $(SRC)