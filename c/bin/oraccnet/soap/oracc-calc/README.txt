This is just a place for notes--James/Martin/anyone else, feel free to dump comments in here ...

*2014-03-14

** installed gsoap with the following friendly exit message:

+--------------------------------------------------------+
| You now have successfully built and installed gsoap.   |
|                                                        |
| You can link your programs with -lgsoap++ for          |
| C++ projects created with soapcpp2 and you can link    |
| with -lgsoap for C projects generated with soapcpp2 -c |
|                                                        |
| There are also corresponding libraries for SSL and     |
| zlib compression support (-lgsoapssl and lgsoapssl++)  |
| which require linking -lssl -lcrypto -lz               |
|                                                        |
| Thanks for using gsoap.                                |
|                                                        |
|               http://sourceforge.net/projects/gsoap2   |
+--------------------------------------------------------+

** first play with building ocalc based on http://www.genivia.com/products/gsoap/demos/calc.html but in C rather than C++, so in Step 2 we need 

	soapcpp2 -c ocalc.h

** compiled the cgi version of the ocalc service with:

	gcc -I/usr/local/include -c soapC.c
	gcc -I/usr/local/include -c soapServerLib.c
	gcc -I/usr/local/include -c soapServer.c
	gcc -I/usr/local/include -c ocalc-server.c

Mac OS X:
	gcc -L/usr/local/lib -lgsoap -o oracc-calc.cgi soapServer.o soapC.o ocalc-server.o

Linux:
	gcc -L/usr/local/lib -o oracc-calc.cgi soapServer.o soapC.o ocalc-server.o -lgsoap -lm

** compiled the client:

	gcc -I/usr/local/include soapClientLib.c
	gcc -I/usr/local/include ocalc-client.c
	gcc -L/usr/local/lib -o ocalc-client -lgsoap oracc-client.o soapC.o soapClient.o
