## Emscripten protobuf sample

Just a sample of compiled box2d with emscripten. It requires cmake to build. Tested on Linux.

```bash
git clone --recurse-submodules https://github.com/NovikovNick/emscripten_box2d.git
```

* **html** - static html files to check compiled wasm. Requires some sort of local html-server to check it properly  
* **src** - contains cpp/js bindings in main.cc 
* **third_parties** - emsdk as git submodules. Box2d as sources. Check ./build.sh

