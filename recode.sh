#!/bin/bash
for a in $(find . -name "*.c"); do iconv --from-code=windows-1251 --to-code=utf-8 "$a" -o "$a"; done
for a in $(find . -name "*.h"); do iconv --from-code=windows-1251 --to-code=utf-8 "$a" -o "$a"; done
dos2unix *.c
dos2unix *.h


