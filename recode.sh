#!/bin/sh
in $(find . -name "*.c"); do iconv --from-code=windows-1251 --to-code=utf-8 <"$a" -o "$a"; done
in $(find . -name "*.h"); do iconv --from-code=windows-1251 --to-code=utf-8 <"$a" -o "$a"; done

