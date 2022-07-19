# lifegame_wasm.cpp
This code provides a WebAssembly code of Conway's Game of Life written in C++.

# Build
```
emcc core.cpp -s WASM=1 -s USE_SDL=2 -O3 -o index.js
```

# Run
```
emrun index.html
```

# Requirements
Please install [Emscripten](https://kripken.github.io/emscripten-site/index.html).

# Reference
- [sdl-canvas-wasm](https://github.com/timhutton/sdl-canvas-wasm)
- [d0iasm/life-game.cpp](https://github.com/d0iasm/life-game.cpp)
