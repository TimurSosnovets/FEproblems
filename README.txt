Cmake пока не освоил, запускаю так:
cd tests
g++ .\Plate_triangulated_test.cpp -o main ../src/mechanical/*.cpp
g++ .\Plate_simple_test.cpp -o main ../src/mechanical/*.cpp
g++ .\DM_2d_static_test.cpp -o main ../../src/thermal/*.cpp
./main.exe