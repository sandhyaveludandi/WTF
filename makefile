all: WTF/WTF.c WTFserver/WTFserver.c
	gcc -lpthread -o WTF ./WTF/WTF.c
	gcc -o WTFserver ./WTFserver/WTFserver.c
clean:
	rm -f WTF/WTF
	rm -f WTFserver/WTFserver
	rm WTFtest
test: WTFtest.c
	gcc -o WTFtest WTFtest.c
