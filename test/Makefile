.PRECIOUS: %.ll %.instrumented.ll

SRC:=$(wildcard *.c)
TARGETS:=$(patsubst %.c, %, $(SRC))

build-all: ${TARGETS}

all: $(patsubst %, dse-%, $(TARGETS))


%: %.c
	 clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $<
	 opt -load ../cmake-build-debug/InstrumentPass.so -DSEInstrument -S $*.ll -o $*.instrumented.ll
	 clang -o $@ -L../cmake-build-debug -lruntime $*.instrumented.ll
    #clang -o $@ -L${PWD}/../cmake-build-debug -lruntime $*.instrumented.ll

dse-%: %
	./test $<

clean:
	rm -f *.ll *.out *.err *.smt2 input.txt branch.txt ${TARGETS}
