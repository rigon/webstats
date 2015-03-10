
all:
	gcc -Wall -O3 -c logparser.c
	gcc -Wall -O3 -c patricia_trie.c
	gcc -Wall -O3 -c ipinfo.c
	gcc -Wall -O3 -c main.c
	gcc -Wall -o webstats -lcurl -ljson-c logparser.o patricia_trie.o ipinfo.o main.o

json:
	gcc -O3 -ljson-c -o json json.c
	
curl:
	gcc -O3 -lcurl -o curl curl.c
	
sqlite:
	gcc -O3 -lsqlite3 -o sqlite sqlite.c
