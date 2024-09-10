#include "../2dGeometry/2dPoint.h"
#include "../2dGeometry/2dVector.h"
#include "../2dGeometry/2dVectors.h"

Vector V1 = Vector(1,2);
Vector V2 = Vector(4,6);
Vector V_perp1 = Vector(-2,1);
double factor = 2.0f;
double angle = M_PI / 2;

// Тест сложения
bool test_add() {
    Vector expected = Vector(5,8);
    if (V1 + V2 == expected) return true;
    else return false;
};

// Тест вычитания
bool test_sub() {
    Vector expected = Vector(3,4);
    if (V2 - V1 == expected) return true;
    else return false;
};

// Тест скалярного умножения
bool test_dot_product() {
    double expected = 16;
    if (V1.dot(V2) == expected) return true;
    else return false;
};

// Тест векторного умножения
bool test_cross_product() {
    double expected = -2;
    if (V1.cross(V2) == expected) return true;
    else return false;
};

// Тест параллельности
bool test_parallel() {
    if ((V1.is_parallel_to(V1)) && !(V1.is_parallel_to(V2))) return true;
    else return false;
};

// Тест перпендикулярности
bool test_perpendicular() {
    if (!(V1.is_perpendicular_to(V1)) && (V1.is_perpendicular_to(V_perp1))) return true;
    else return false;
};

// Тест скалирования
bool test_scaled_by() {
    Vector expected = Vector(2,4);
    if (V1.scaled_by(factor) == expected) return true;
    else return false;
};

// Тест нормализации
bool test_normalized() {
    Vector expected1 = Vector(1/sqrt(5),2/sqrt(5));
    Vector expected2 = Vector(factor*1/sqrt(5),factor*2/sqrt(5));
    if ((V1.normalized() == expected1) && (V1.with_length(factor)) == expected2) return true;
    else return false;
};

// Тест угла
bool test_angle_to() {
    double expected = M_PI/2;
    if (V1.angle_to(V_perp1) == expected) return true;
    else return false;
};

// Тест поворота
bool test_rotated_radians() {
    Vector expected = V_perp1;
    if (V1.rotated_radians(angle) == expected) return true;
    else return false;
};

// Тест проекции
bool test_projection_over() {
    double expected = 0;
    if (V1.projection_over(V_perp1) == expected) return true;
    else return false;
};

using namespace std;
int main() {
 if (test_add()) cout << "Success! The +operator works correctly!" << endl;
   else { 
    cout << "Error detected! The +operator works incorrectly. Here are details: \n"; 
    cout << "Vector 1: ";
    V1.get_info(); 
    cout << "Vector 2: ";
    V2.get_info(); 
    cout << "Vector 1 + Vector 2: ";
    (V1+V2).get_info(); 
   }; 
   cout << endl;

 if (test_sub()) cout << "Success! The -operator works correctly!" << endl;
   else { 
    cout << "Error detected! The -operator works incorrectly. Here are details: \n"; 
    cout << "Vector 1: ";
    V1.get_info(); 
    cout << "Vector 2: ";
    V2.get_info(); 
    cout << "Vector 2 - Vector 2: ";
    (V2-V1).get_info(); 
   }; 
   cout << endl;

 if (test_dot_product()) cout << "Success! The dot_product method works correctly!" << endl;
    else {
     cout << "Error detected! The dot_product method works incorrectly. Here are details: \n"; 
     cout << "Vector 1: ";
     V1.get_info(); 
     cout << "Vector 2: ";
     V2.get_info(); 
     cout << "Vector 1 * Vector 2: " << V1.dot(V2) << endl;
     };
    cout << endl;

 if (test_cross_product()) cout << "Success! The cross_product method works correctly!" << endl;
    else {
     cout << "Error detected! The cross_product method works incorrectly. Here are details: \n"; 
     cout << "Vector 1: ";
     V1.get_info(); 
     cout << "Vector 2: ";
     V2.get_info(); 
     cout << "Vector 1 x Vector 2: " << V1.cross(V2) << endl;
    };
    cout << endl;

 if (test_parallel()) cout << "Success! The is_parallel_to method works correctly!" << endl;
    else {
        cout << "Error detected! The is_parallel_to method works incorrectly. Here are details: \n";
     cout << "Vector 1: ";
     V1.get_info(); 
     cout << "Vector 2: ";
     V2.get_info(); 
     cout << "Vector 1 x Vector 1: " << V1.cross(V1) << endl;
     cout << "Vector 1 x Vector 2: " << V1.cross(V2) << endl;
    };
    cout << endl;

 if (test_perpendicular()) cout << "Success! The is_perpendicular_to method works correctly!" << endl;
    else {
        cout << "Error detected! The is_perpendicular_to method works incorrectly. Here are details: \n";
     cout << "Vector 1: ";
     V1.get_info(); 
     cout << "Vector 2: ";
     V_perp1.get_info(); 
     cout << "Vector 1 * Vector 1: " << V1.dot(V1) << endl;
     cout << "Vector 1 * Vector 2: " << V1.dot(V_perp1) << endl;
    };
    cout << endl;

    if (test_scaled_by()) cout << "Success! The scaled_by method works correctly!" << endl;
    else {
     cout << "Error detected! The scaled_by method works incorrectly. Here are details: \n";
     cout << "Vector 1: ";
     V1.get_info(); 
     cout << "Scale factor: " << factor << endl;
     cout << "Vector 1 scaled by factor: ";
     V1.scaled_by(factor).get_info();
    };
    cout << endl;

    if (test_normalized()) cout << "Success! The normalized method works correctly!" << endl;
    else {
     cout << "Error detected! The normalized method works incorrectly. Here are details: \n";
     cout << "Vector 1: ";
     V1.get_info(); 
     cout << "Vector 1 normalized: ";
     V1.normalized().get_info();
     cout << "New length: " << factor << endl;
     V1.with_length(factor).get_info();
     };
    cout << endl;

    if (test_angle_to()) cout << "Success! The angle_to method works correctly!" << endl;
    else {
     cout << "Error detected! The angle_to method works incorrectly. Here are details: \n";
     cout << "Vector 1: ";
     V1.get_info(); 
     cout << "Vector 2: ";
     V_perp1.get_info(); 
     cout << "Angle between Vector 1 and Vector 2 is: " << V1.angle_to(V_perp1) << "Expected: " << M_PI/2 << endl;
    };
    cout << endl;

    if (test_rotated_radians()) cout << "Success! The rotated_radians method works correctly!" << endl;
    else {
     cout << "Error detected! The rotated_radians method works incorrectly. Here are details: \n";
     cout << "Vector 1: ";
     V1.get_info(); 
     cout << "Rotate angle: " << angle << endl;
     cout << "New vector: ";
     V1.rotated_radians(angle).get_info(); 
     };
    cout << endl;

    if (test_projection_over()) cout << "Success! The projection_over method works correctly!" << endl;
    else {
     cout << "Error detected! The projection_over method works incorrectly. Here are details: \n"; 
     cout << "Vector 1: ";
     V1.get_info(); 
     cout << "Vector 2: ";
     V_perp1.get_info(); 
     cout << "Projection of Vector 1 over Vector 2 is: " << V1.projection_over(V2) << endl;
     };
    cout << endl;

    Point p1 = Point(0,0);
    Point p2 = Point(1,1);
     Vector Vec = make_vector_between(p1,p2);
return 0;
};