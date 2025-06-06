DESTDIR?=
PREFIX?=/usr/local
INSTALL ?= install

notedriller:	notedriller.c
	gcc -Wall -Wextra -Wstrict-prototypes -o notedriller notedriller.c

clean:
	rm -f notedriller

install:	notedriller notedriller.1
	@mkdir -p ${DESTDIR}${PREFIX}
	${INSTALL} -m 755 notedriller ${DESTDIR}${PREFIX}/bin
	${INSTALL} -m 644 notedriller.1 ${DESTDIR}${PREFIX}/man/man1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/notedriller
	rm -f ${DESTDIR}${PREFIX}/man/man1/notedriller.1

