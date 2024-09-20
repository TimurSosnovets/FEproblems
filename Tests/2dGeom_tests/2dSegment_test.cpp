#include "../../2dGeometry/2dSegment.h"

using namespace std;

// Точки
Point m_start = Point(400,0);
Point m_end = Point(0,400);
Point closest_start = Point(500,20);
Point closest_middle = Point(250,250);
Point closest_end = Point(20,500);
Point m_middle = Point(200,200);
// Числа
double test_t =  0.25;
// Сегменты
Segment main_segment = Segment(m_start,m_end);
Segment parallel = Segment(Point(200,0), Point(0,200));
Segment intersection = Segment(Point(0,0), Point(400,400));



// Тест длины
bool test_length() {
    double expected = 400*sqrt(2);
    if (main_segment.get_length() == expected) return true;
    else return false;
};

// Тест получения точки на сегменте
bool test_point_at() {
    double t = make(test_t);
    Point expected = Point(300,100);
    if (main_segment.point_at(t) == expected) return true;
    else return false;
};

// Тест средней точки
bool test_middle_point() {
    Point expected = Point(200,200);
    if (main_segment.get_middle() == expected) return true;
    else return false;
};

// Тест ближайшей точки
bool test_closest_point_is_start() {
    Point expected = m_start;
    if (main_segment.closest_point_to(closest_start) == expected) return true;
    else return false;
};

bool test_closest_point_is_end() {
    Point expected = m_end;
    if (main_segment.closest_point_to(closest_end) == expected) return true;
    else return false;
};

bool test_closest_point_is_middle() {
    Point expected = m_middle;
    if (main_segment.closest_point_to(closest_middle) == expected) return true;
    else return false;
};

// Тест пересечения сегментов
bool test_parallel_segments_no_intersection() {
    return !(main_segment.is_intersection_with(parallel));
};

bool test_segments_intersection() {
    Point expected = m_middle;
    if ((main_segment.is_intersection_with(intersection)) && (main_segment.intersection_with(intersection) == expected)) return true;
    else return false;
};

int main() {
cout << endl << endl << endl;
if (test_length()) cout << "Success! The length property works correctly!" << endl;
   else { 
    cout << "Error detected! The length property works incorrectly. Here are details: \n"; 
    cout << "Segment 1: ";
    main_segment.get_info(); 
    }; 
   cout << endl;

if (test_point_at()) cout <<  "Success! The point_at method works correctly!" << endl;
    else {
        cout << "Error detected! The point_at method works incorrectly. Here are details: \n"; 
        cout << "Segment 1: ";
        main_segment.get_info();
        cout << "Point with t = " << test_t << ":" << endl;
        main_segment.point_at(test_t).get_info();
        cout << endl;
        };
    cout << endl;

if (test_middle_point()) cout << "Success! The middle property works correctly!" << endl;
    else { 
        cout << "Error detected! The middle property works incorrectly. Here are details: \n"; 
        cout << "Segment 1: ";
        main_segment.get_info(); 
        cout << "Middle is: ";
        main_segment.get_middle().get_info();
        cout << endl;
    }; 
   cout << endl;

if ((test_closest_point_is_start()) && (test_closest_point_is_end()) && (test_closest_point_is_middle())) cout << "Success! The closest_point_to method works correctly!" << endl;
    else { 
        cout << "Error detected! The closest_point_to method works incorrectly. Here are details: \n"; 
        cout << "Segment 1: ";
        main_segment.get_info(); 

        cout << "Point 1: ";
        closest_start.get_info(); cout << endl;
        cout << "Closest point of segment 1: ";
        main_segment.closest_point_to(closest_start).get_info(); cout << endl;
        cout << endl;

        cout << "Point 2: ";
        closest_end.get_info(); cout << endl;
        cout << "Closest point of segment 1: ";
        main_segment.closest_point_to(closest_end).get_info(); cout << endl;
        cout << endl;

        cout << "Point 3: ";
        closest_middle.get_info(); cout << endl;
        cout << "Closest point of segment 1: ";
        main_segment.closest_point_to(closest_middle).get_info(); cout << endl;
        cout << endl;
    }; 
   cout << endl;

if ((test_parallel_segments_no_intersection()) && (test_segments_intersection())) cout << "Success! The intersection_with method works correctly!" << endl;
    else { 
        cout << "Error detected! The intersection_with method works incorrectly. Here are details: \n"; 
        cout << "Segment 1: ";
        main_segment.get_info(); 

        cout << "Parallel Segment: ";
        parallel.get_info();
        cout << "Is intersection?: ";
        if (main_segment.is_intersection_with(parallel)) {
            std::cout << "Yes: ";
            main_segment.intersection_with(parallel).get_info(); cout << endl;
        }
        else {
            std::cout << "No." << endl;
            cout << "t1 = " << main_segment.p_intersection_coord(parallel).first << ";  t2 = " << main_segment.p_intersection_coord(intersection).second << ";\n";
        };

        cout << "Intersection Segment: ";
        intersection.get_info();
        cout << "Is intersection?: ";
        if (main_segment.is_intersection_with(intersection)) {
            std::cout << "Yes: ";
            main_segment.intersection_with(intersection).get_info(); cout << endl;
        }
        else {
            std::cout << "No." << endl;
            cout << "t1 = " << main_segment.p_intersection_coord(intersection).first << ";  t2 = " << main_segment.p_intersection_coord(intersection).second << ";\n";
            main_segment.intersection_with(intersection).get_info(); cout << endl;
        };
    }; 
   cout << endl;
return 0;
};