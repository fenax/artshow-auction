all:  list edit

list:	list.c
	cc -g list.c `pkg-config --cflags gtk+-3.0` `pkg-config --cflags sqlite3`  `pkg-config --libs gtk+-3.0` `pkg-config --libs sqlite3` -o list.out

edit:	edit.c
	cc -g edit.c `pkg-config --cflags gtk+-3.0` `pkg-config --cflags sqlite3`  `pkg-config --libs gtk+-3.0` `pkg-config --libs sqlite3` -o edit.out

main:	main.c
	cc -g main.c `pkg-config --cflags gtk+-3.0` `pkg-config --cflags sqlite3`  `pkg-config --libs gtk+-3.0` `pkg-config --libs sqlite3`
