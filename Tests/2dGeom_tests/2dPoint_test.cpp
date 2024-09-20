#include "../../2dGeometry/2dPoint.h"
#include "../../2dGeometry/2dVector.h"

Point p1 = Point(1,2);
Point p2 = Point(4,6);
Vector V = Vector(4,6);
Point p_dist1 = Point(4,0);
Point p_dist2 = Point(0,3);

// Тест сложения
bool test_add() {
    Point expected = Point(5,8);
  if (p1+p2 == expected) return true;
  else return false;
};

// Тест вычитания
bool test_sub() {
  Vector expected = Vector(3,4);
  if (p2 - p1 == expected) return true;
  else return false;
};

// Тест перемещения
bool test_displaced() {
  Vector V = Vector(4,6);
  Point expected1 = Point(5,8);
  Point expected2 = Point(9,14);
  if ((p1.displaced(V) == expected1) && (p1.displaced(V,2.0f) == expected2)) return true;
  else return false;
};

// Тест расстояния
bool test_distacnce_to() {
  double expected = 5.0f;
  if (p_dist1.distance_to(p_dist2) == expected) return true;
  else return false;
};
 int main () {
   if (test_add()) cout << "Success! The +operator works correctly!" << endl;
   else { 
    cout << "Error detected! The +operator works incorrectly. Here are details: \n"; 
    cout << "Point 1: ";
    p1.get_info(); cout << endl;
    cout << "Point 2: ";
    p2.get_info(); cout << endl;
    cout << "Point 1 + Point 2: ";
    (p1+p2).get_info(); cout << endl;
   }; 
   cout << endl;

   if (test_sub()) cout << "Success! The -operator works correctly!" << endl;
   else { 
    cout << "Error detected! The -operator works incorrectly. Here are details: \n"; 
    cout << "Point 1: ";
    p1.get_info(); cout << endl;
    cout << "Point 2: ";
    p2.get_info(); cout << endl;
    cout << "Point 2 - Point 1: ";
    (p2-p1).get_info(); 
   };
    cout << endl;

   if (test_displaced()) cout << "Success! The displaced method works correctly!" << endl;
   else {
    cout << "Error detected! The displaced method works incorrectly. Here are details: \n"; 
    cout << "Point 1: ";
    p1.get_info(); cout << endl;
    V.get_info();
    cout << "Point 1 displased by Vector Vx1 is ";
    p1.displaced(V).get_info(); cout << endl;
    cout << "Point 1 displased by Vector Vx2 is ";
    p1.displaced(V,2).get_info(); cout << endl;
   };
   cout << endl;

   if (test_distacnce_to()) cout << "Success! The distance_to method works correctly!" << endl;
   else {
    cout << "Error detected! The distance_to method works incorrectly. Here are details: \n";
    cout << "Point 1: ";
    p_dist1.get_info(); cout << endl;
    cout << "Point 2: ";
    p_dist2.get_info(); cout << endl;
    cout << "Distance between points 1 and 2 is: " << p_dist1.distance_to(p_dist2) << endl;
   };
   cout << endl;
     return 0;
}