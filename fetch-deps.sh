#!/bin/bash

if [ ! -f dlib-19.4.tar.bz2 ]; then
  echo "Fetching dlib-19.4"
  curl http://dlib.net/files/dlib-19.4.tar.bz2 > dlib-19.4.tar.bz2
fi

if [ ! -f lua-5.3.4.tar.gz ]; then
  echo "Fetching lua-5.3.4"
  curl https://www.lua.org/ftp/lua-5.3.4.tar.gz > lua-5.3.4.tar.gz
fi

tar xvjf dlib-19.4.tar.bz2
tar xvzf lua-5.3.4.tar.gz