# compile commands (using clang-16 and libc++)

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-stdlib=libc++" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
make -j$(nproc)
```
