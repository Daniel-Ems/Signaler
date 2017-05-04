CPPFLAGS+=-Wall -Wextra -Wpedantic -Wextra -Wpedantic -Wwrite-strings 
CPPFLAGS+=-Wstack-usage=1024 -Wfloat-equal -Waggregate-return -Winline

CFLAGS+=-std=c11
LDLIBS+=-pthread

all=signaler
bins: $(all)

signaler: signaler.o 



.PHONY: clean debug profile




debug: CLFAGS+=-g
debug: $(all)

profile: CFLAGS+=-pg
profile: LDFLAGS+=-pg
profile: $(bins)


clean:
	
