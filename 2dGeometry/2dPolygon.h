#pragma once
#include <optional>
#include <numeric>
#include <algorithm>
#include <vector>
#include "2dSegment.h"
#include "Utils/pairs.h"

using namespace std;

class Polygon {
    private:
    std::vector<Point> vertices;
    std::vector<Segment> sides;
    std::optional<Point> centroid;

    public:

        friend bool operator== (const Polygon& p1, const Polygon& p2);

        void get_info() {
            int lenght = sides.size();
            cout << "Polygon: \n";
            for (int i = 0; i < lenght; ++i) {
                sides[i].get_info();
            };
        }

        std::optional<Point> get_centroid() {
            return centroid;
        }
        
        std::vector<Segment> get_sides() {
            return sides;
        }

        void set_centroid() {
            int vtx_count = vertices.size();
            Point vtx_sum = std::accumulate(vertices.begin(), vertices.end(), Point(0,0));
            centroid = Point(vtx_sum.x / vtx_count, vtx_sum.y / vtx_count);
        }

        
        Polygon(const std::vector<Point>& set_points): vertices(set_points) {
            if (this->vertices.size() < 3) {
                throw std::invalid_argument("Need 3 or more vertices");
            };
            auto vertex_pairs = make_round_pairs(vertices);
            sides.reserve(vertex_pairs.size());
            for (const auto& points : vertex_pairs) {
                sides.emplace_back(Segment(points.first, points.second));
            };
            set_centroid();
        }

        bool contains_point(const Point position) const {
            // Проверка, является ли точка вершиной
            auto it = std::find(vertices.begin(), vertices.end(), position);
            if (it != vertices.end()) return true;
            if (position == *vertices.end()) return true;
            // Основная часть
            std::vector<Vector> vecs;
            vecs.reserve(vertices.size());
            for (const auto& vertex: vertices) {
                vecs.emplace_back(make_vector_between(position, vertex));
            };
            auto paired_vecs = make_round_pairs(vecs);
            double angle_sum = 0.0f;
            for (const auto& vec_pair: paired_vecs) {
                angle_sum += vec_pair.first.angle_to(vec_pair.second);
            }
            return are_close_enough(angle_sum, 2*M_PI);
        }
};

bool operator== (const Polygon& p1, const Polygon& p2) {
    return (p1.vertices == p2.vertices);
};