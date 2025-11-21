cmake -S . -B build-devenv -G Ninja -DCMAKE_BUILD_TYPE=Debug -DUSE_SYSTEM_Z3=ON
cmake --build build-devenv --target Verifier
./build-devenv/Verifier