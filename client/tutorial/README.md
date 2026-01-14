# Tutorials

## Installations
`brew install curl`
`brew install cjson`

## Compile and Run
### Libcurl tutorial
Make sure to link the libcurl
`gcc -o libcurl_tut libcurl_tut.c -lcurl`

### cJSON tutorial
Make sure to link the cjson lib
`gcc -o cjson_tut cjson_tut.c -lcjson -I/opt/homebrew/include -L/opt/homebrew/lib`

Note: `-I` is for headers and `-L` is for libraries

### Combined

### nCurses
