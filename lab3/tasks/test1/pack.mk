PROGS_TEST1_OBJS := test1.o
PROGS_TEST1_OBJS := $(PROGS_TEST1_OBJS:%=$(TDIR)/test1/%)
ALL_OBJS += $(PROGS_TEST1_OBJS)

$(TDIR)/bin/test1 : $(TSTART) $(PROGS_TEST1_OBJS) $(TLIBC)
