rmdir /s /q build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j %NUMBER_OF_PROCESSORS%
