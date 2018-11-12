emcc ^
    -w ^
    -O2 ^
    -std=c99 ^
    -fdeclspec ^
    -o www/index.html ^
    -s WASM=1 ^
    -s USE_WEBGL2=1 ^
    -s USE_GLFW=3 ^
    -s FULL_ES3=1 ^
    -s USE_FREETYPE=1 ^
    --embed-file assets/text.frag ^
    --embed-file assets/text.vs ^
    --preload-file assets/LiberationSansBold.ttf ^
    --use-preload-plugins ^
    src/utils.c ^
    src/main.c