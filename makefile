all: WTF.c WTFserver.c
	gcc -lpthread -o WTF WTF.c
	gcc -o WTFserver WTFserver.c
clean:
	rm WTF
	rm WTFserver
	rm WTFtest
test: WTFtest.c
	gcc -o WTFtest WTFtest.c