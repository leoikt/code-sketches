#include <stdio.h>
#include <math.h>
#include <assert.h>

double distBtwDots(const double* pts) {
    double dx = pts[2] - pts[0]; // x2-x1
    double dy = pts[3] - pts[1]; // y2-y1
    return (double)hypot(dx, dy);
}
/* вычисляем евклидово расстояние между
 * двумя точками на плоскости через гипотенузу 
 * треугольника, выстроенного между точками.
 * hypot() минимизирует риски относительно прямого вычисления по формуле 
*/

int main(void) {
    double points[4] = {}, *p = points;
    assert(scanf("%lf%lf%lf%lf", p, p+1, p+2, p+3) == 4);
    printf("%.2lf", distBtwDots(p));
    return 0;
}
