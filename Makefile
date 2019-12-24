# Set path to compiled XML RPC library here
XMLRPCROOT=/home/user/Desktop/md_asa_lib/xmlrpc-c-1.43.08

XMLRPCPATHS=
XMLRPCPATHS+=-L$(XMLRPCROOT)/lib/expat/xmlparse
XMLRPCPATHS+=-L$(XMLRPCROOT)/lib/expat/xmltok
XMLRPCPATHS+=-L$(XMLRPCROOT)/lib/libutil
XMLRPCPATHS+=-L$(XMLRPCROOT)/src

CC=gcc

all:libasa.so asaexample

libasa.so: src/asa.c
	$(CC) -c -I./include -I$(XMLRPCROOT)/include -fPIC src/asa.c
	$(CC) -c -I./include -I$(XMLRPCROOT)/include -fPIC libasa/compare_ivc.c
	$(CC) -shared -o libasa.so asa.o compare_ivc.o -lxmlrpc_client -lxmlrpc -lxmlrpc_xmlparse -lxmlrpc_xmltok -lxmlrpc_util -lpthread  -L/usr/lib/x86_64-linux-gnu $(XMLRPCPATHS) -lcurl

asaexample: example/asaexample.c libasa.so
	$(CC) -c -I./include example/asaexample.c
	$(CC) -o asaexample asaexample.o -lasa -lxmlrpc_client -lxmlrpc -lxmlrpc_xmlparse -lxmlrpc_xmltok -lxmlrpc_util -lpthread  -L/usr/lib/x86_64-linux-gnu -L. $(XMLRPCPATHS) -lcurl -lm

clean:
	rm asaexample.o asa.o compare_ivc.o libasa.so asaexample
