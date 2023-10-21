#ifndef TRIANGLE_AREA_CALCULATOR_H
#define TRIANGLE_AREA_CALCULATOR_H

#include <cmath>

struct Point3D {
    double x;
    double y;
    double z;
};

class Triangle3D {
public:
    Triangle3D(Point3D p1, Point3D p2, Point3D p3) : point1(p1), point2(p2), point3(p3) {}

    double calculateArea() const {
        // Calculate the vectors between the points
        Point3D v1 = subtractPoints(point2, point1);
        Point3D v2 = subtractPoints(point3, point1);

        // Calculate the cross product of the two vectors
        Point3D crossProduct = crossProductOfVectors(v1, v2);

        // Calculate the area of the triangle
        double area = 0.5 * vectorMagnitude(crossProduct);
        return area;
    }

private:
    Point3D point1;
    Point3D point2;
    Point3D point3;

    Point3D subtractPoints(Point3D p1, Point3D p2) const {
        Point3D result;
        result.x = p1.x - p2.x;
        result.y = p1.y - p2.y;
        result.z = p1.z - p2.z;
        return result;
    }

    Point3D crossProductOfVectors(Point3D v1, Point3D v2) const {
        Point3D result;
        result.x = v1.y * v2.z - v1.z * v2.y;
        result.y = v1.z * v2.x - v1.x * v2.z;
        result.z = v1.x * v2.y - v1.y * v2.x;
        return result;
    }

    double vectorMagnitude(Point3D v) const {
        return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }
};

#endif // TRIANGLE_AREA_CALCULATOR_H