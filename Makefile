CC		= m68k-amigaos-gcc
CFLAGS		=		\
	-std=c99		\
	-Werror			\
	-Wall			\
	-Wextra			\
	-Wno-pointer-sign	\
	-O3			\
	-m68020			\
	-s			\
	-DNO_INLINE_STDARG
DEPFLAGS	= -MT $@ -MMD -MP -MF $(BUILDDIR)/$*.Td
BUILDDIR	= build
PARTEST		= $(BUILDDIR)/ParTest
PARTEST_SRCS	=		\
	common.c		\
	main.c			\
	parallel.c		\
	ui.c
PARTEST_OBJS	= $(patsubst %, $(BUILDDIR)/%.o, $(PARTEST_SRCS))

$(shell mkdir -p $(BUILDDIR) >/dev/null)

all: $(PARTEST)

clean:
	rm -fr $(BUILDDIR)

emubuild: $(PARTEST)
	@cp $< "/mnt/c/Users/Public/Documents/Amiga Files/WinUAE/HDD/Data/"

$(PARTEST): $(PARTEST_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

$(BUILDDIR)/%.c.o : %.c $(BUILDDIR)/%.d
	$(CC) $(DEPFLAGS) $(CFLAGS) -c -o $@ $<
	@mv -f $(BUILDDIR)/$*.Td $(BUILDDIR)/$*.d && touch $@

$(BUILDDIR)/%.d: ;

.PRECIOUS: $(BUILDDIR)/%.d

include $(wildcard $(patsubst %, $(BUILDDIR)/%.d, $(basename $(PARTEST_SRCS))))

.PHONY: $(DISTDIR)
