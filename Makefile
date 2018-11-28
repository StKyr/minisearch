CC = gcc
CCEXTRAS = 
CCFLAGS = -Iinclude
VPATH = src:bin:samples:include
OBJECTIVES = ./bin/main.o ./bin/jobExecutor.o ./bin/commandParsing.o ./bin/worker.o ./bin/search.o ./bin/Map.o ./bin/postingList.o ./bin/Trie.o ./bin/util.o


jobExecutor: $(OBJECTIVES)
	$(CC) $(CCFLAGS) -o ./jobExecutor $(OBJECTIVES) $(CCEXTRAS)

./bin/main.o: ./src/main.c
	$(CC) $(CCFLAGS) -c ./src/main.c -o ./bin/main.o

./bin/jobExecutor.o: ./src/jobExecutor.c jobExecutor.h
	$(CC) $(CCFLAGS) -c  ./src/jobExecutor.c -o ./bin/jobExecutor.o

./bin/commandParsing.o: ./src/commandParsing.c commandParsing.h 	

./bin/worker.o: ./src/worker.c worker.h
	$(CC) $(CCFLAGS) -c ./src/worker.c -o ./bin/worker.o

./bin/search.o: ./src/search.c search.h
	$(CC) $(CCFLAGS) -c ./src/search.c -o ./bin/search.o

./bin/Map.o: ./src/Map.c Map.h
	$(CC) $(CCFLAGS) -c ./src/Map.c -o ./bin/Map.o

./bin/postingList.o: ./src/postingList.c postingList.h
	$(CC) $(CCFLAGS) -c ./src/postingList.c -o ./bin/postingList.o

./bin/Trie.o: ./src/Trie.c Trie.h
	$(CC) $(CCFLAGS) -c ./src/Trie.c -o ./bin/Trie.o

./bin/util.o: ./src/util.c util.h
	$(CC) $(CCFLAGS) -c ./src/util.c -o ./bin/util.o

.PHONY: clean

clean:
	rm -f $(OBJECTIVES) ./jobExecutor