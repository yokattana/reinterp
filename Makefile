CFLAGS += -Wall

.PHONY: clean

all: reinterp

clean:
	$(RM) reinterp
	$(RM) reinterp.exe
