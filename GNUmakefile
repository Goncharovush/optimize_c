INCLUDES=-I/c/c++/asa_lib/md_asa_lib/include -I/c/c++/asa_lib/md_asa_lib/ -I/c/c++/asa_lib/md_asa_lib/xmlrpc-c-1.43.08/include
LIBRARIES=-L/c/c++/asa_lib/md_asa_lib/
CFLAGS=-m32
DEBUGCFLAGS=-m32 -g -O0 -fno-omit-frame-pointer
LIBRARI=-L/c/c++/asa_lib/md_asa_lib/xmlrpc-c-1.43.08/bin/Release-DLL-Win32
DLLPATH=/c/c++/asa_lib/md_asa_lib/xmlrpc-c-1.43.08/bin/Release-DLL-Win32



all: libasa.dll asaexample

libasa.dll: src/asa.c libasa/compare_ivc.c
	gcc -c ${CFLAGS} ${INCLUDES} libasa/compare_ivc.c -o compare_ivc.o
	gcc -c ${CFLAGS} ${INCLUDES} src/asa.c -o asa.o
	gcc -shared ${CFLAGS} ${INCLUDES} asa.o compare_ivc.o -o libasa.dll ${LIBRARI} -lxmlrpc -lxmlrpc_client -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok 

asaexample:example/asaexample.c libasa.dll
	gcc ${CFLAGS} ${INCLUDES} ${LIBRARIES} -llibasa ${LIBRARI} -lxmlrpc -lxmlrpc_client -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok example/asaexample.c -o asaexample.exe
