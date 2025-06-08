.DEFAULT_GOAL := build

build:
	sudo pacman -S gtk3 vte3 pango cairo gdk-pixbuf2 ttf-roboto-mono  --noconfirm
	gcc -o term term.c -I/usr/include/vte-2.91 -I/usr/include/gtk-3.0 -I/usr/include/pango-1.0 -I/usr/include/cloudproviders -I/usr/include/cairo -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/fribidi -I/usr/include/harfbuzz -I/usr/include/gio-unix-2.0 -I/usr/include/freetype2 -I/usr/include/pixman-1 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/atk-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-6 -pthread -L/usr/lib -lpthread -lm -lgtk-3 -lvte-2.91 -lglib-2.0 -lgobject-2.0 -lgdk-3 -lpango-1.0

install:
	sudo cp term /usr/local/bin/gwmct
	sudo chmod 755 /usr/local/bin/gwmct
	sudo chown :root /usr/local/bin/gwmct
	mkdir -p ~/.config/gwmct
	cp configfile/config.ini ~/.config/gwmct
	sudo cp gwmct.desktop /usr/share/applications/gwmct.desktop   
