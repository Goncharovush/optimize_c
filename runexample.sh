#!/bin/bash
# Set path to compiled XML RPC library here
XMLRPCROOT="/home/user/Desktop/md_asa_lib/xmlrpc-c-1.43.08"

XMLRPCPATHS="${XMLRPCROOT}/lib/expat/xmlparse"
XMLRPCPATHS+=":${XMLRPCROOT}/lib/expat/xmltok"
XMLRPCPATHS+=":${XMLRPCROOT}/lib/libutil"
XMLRPCPATHS+=":${XMLRPCROOT}/src"

export LD_LIBRARY_PATH="${XMLRPCPATHS}:.:${LD_LIBRARY_PATH}"

if [ -f "./asaexample" ]; then
    ./asaexample
else 
    echo "Run \"make all\" first"
fi
