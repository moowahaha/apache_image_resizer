all:
	gcc src/image_resizer.c -o image_resizer.cgi -lcgic -lgd

clean:
	rm -f image_resizer.cgi
