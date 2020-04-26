CFLAGS     = -Wall -W -I. -Isrc -I../src
TESTS      = tests/CsvEol.o tests/OpenCre.o tests/DiffExamples.o tests/PoolExercise.o tests/PathCanonExamples.o tests/HashmapExercise.o tests/HashmapCount.o tests/LinkedlistExercise.o tests/CsvExamples.o tests/StackExercise.o tests/VarrayOps.o tests/CfgOps.o tests/TcharAll.o tests/TextTest.o tests/ShelloutSimple.o tests/EvalExamples.o tests/SvsemCounter.o tests/SubaExercise.o tests/AdtInit.o tests/SharedAdts.o tests/CfgQueryStringExamples.o tests/CfgSuba.o tests/BitsetOps.o tests/SvcondProdCons.o tests/HashmapCheck.o
VALGRIND   = -v --num-callers=20 --leak-check=yes --leak-resolution=high --show-reachable=yes

all: $(TESTS) tmba.c common.c ../libmba.a
	$(CC) $(CFLAGS) -lutil -o tmba tmba.c common.c $(TESTS) ../libmba.a

.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $<

vg:
	valgrind $(VALGRIND) ./tmba 5300
cg:
	calltree ./tmba 5700

tone: tone.c tests/CfgOps.c
	gcc -g -Wall -W -DMSGNO -DUSE_WCHAR -o tone tone.c tests/CfgOps.c ../libmba.a
run1:
	valgrind $(VALGRIND) ./tone

clean:
	rm -f tmba $(TESTS)
