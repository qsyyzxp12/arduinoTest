all:
	./compile resistanceReader.c
	gcc resistanceReaderController.c -o resistanceReaderController
	rm /bin/resistanceReaderController
	cp resistanceReaderController /bin/
