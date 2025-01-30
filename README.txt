Cmake пока не освоил, запускаю так:
cd tests
g++ .\Plate_triangulated_test.cpp -o main ../src/mechanical/*.cpp
g++ .\Plate_simple_test.cpp -o main ../src/mechanical/*.cpp
g++ .\DM_2d_static_test.cpp -o main ../../src/thermal/*.cpp
g++ .\LQuad_test.cpp -o main ../../src/thermal/*.cpp
g++ .\Plate_lam_evo_test.cpp -o main ../../src/thermal/*.cpp
g++ .\Plate_lam_evo_test.cpp -o main ../../src/thermal/*.cpp -I../../headers/SimpleXlsx -L../../lib/build -lSimpleXlsx


g++ .\testXLDateTime.cpp -o main  -I../../lib/OpenXLSX/headers -L../../lib/OpenXLSX/libOpenXLSX.a -lOpenXLSX
g++ .\Demo1.cpp -o main  -I../../lib/OpenXLSX/headers -L../../lib/OpenXLSX -lOpenXLSX
./main.exe