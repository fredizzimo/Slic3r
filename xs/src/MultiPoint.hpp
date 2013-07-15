#ifndef slic3r_MultiPoint_hpp_
#define slic3r_MultiPoint_hpp_

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"
}

#include "Point.hpp"
#include <algorithm>
#include <vector>

namespace Slic3r {

class MultiPoint
{
    public:
    Points points;
    void from_SV(SV* poly_sv);
    void from_SV_check(SV* poly_sv);
    SV* to_SV();
    SV* to_SV_pureperl();
    void scale(double factor);
    void translate(double x, double y);
    void rotate(double angle, Point* center);
    void reverse();
};

void
MultiPoint::scale(double factor)
{
    for (Points::iterator it = points.begin(); it != points.end(); ++it) {
        (*it).scale(factor);
    }
}

void
MultiPoint::translate(double x, double y)
{
    for (Points::iterator it = points.begin(); it != points.end(); ++it) {
        (*it).translate(x, y);
    }
}

void
MultiPoint::rotate(double angle, Point* center)
{
    for (Points::iterator it = points.begin(); it != points.end(); ++it) {
        (*it).rotate(angle, center);
    }
}

void
MultiPoint::reverse()
{
    std::reverse(this->points.begin(), this->points.end());
}

void
MultiPoint::from_SV(SV* poly_sv)
{
    AV* poly_av = (AV*)SvRV(poly_sv);
    const unsigned int num_points = av_len(poly_av)+1;
    this->points.resize(num_points);
    
    for (unsigned int i = 0; i < num_points; i++) {
        SV** point_sv = av_fetch(poly_av, i, 0);
        this->points[i].from_SV_check(*point_sv);
    }
}

void
MultiPoint::from_SV_check(SV* poly_sv)
{
    if (sv_isobject(poly_sv) && (SvTYPE(SvRV(poly_sv)) == SVt_PVMG)) {
        *this = *(MultiPoint*)SvIV((SV*)SvRV( poly_sv ));
    } else {
        this->from_SV(poly_sv);
    }
}

SV*
MultiPoint::to_SV() {
    const unsigned int num_points = this->points.size();
    AV* av = newAV();
    av_extend(av, num_points-1);
    for (unsigned int i = 0; i < num_points; i++) {
        SV* sv = newSV(0);
        sv_setref_pv( sv, "Slic3r::Point", new Point(this->points[i]) );
        av_store(av, i, sv);
    }
    return newRV_noinc((SV*)av);
}

SV*
MultiPoint::to_SV_pureperl() {
    const unsigned int num_points = this->points.size();
    AV* av = newAV();
    av_extend(av, num_points-1);
    for (unsigned int i = 0; i < num_points; i++) {
        av_store(av, i, this->points[i].to_SV_pureperl());
    }
    return newRV_noinc((SV*)av);
}

}

#endif
