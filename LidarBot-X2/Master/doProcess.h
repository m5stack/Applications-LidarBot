#ifndef __DOPROCESS_H
#define __DOPROCESS_H

//#define M_PI 3.14

static inline float from_degrees(float degrees)
{
    return degrees * M_PI / 180.0;
}

static inline float normalize_angle_positive(float angle)
{
    return fmod(fmod(angle, 2.0*M_PI) + 2.0*M_PI, 2.0*M_PI);
}

static inline float to_degrees(float radians)
{
    return radians * 180.0 / M_PI;
}

static inline float normalize_angle(float angle)
{
    float a = normalize_angle_positive(angle);
    if (a > M_PI)
      a -= 2.0 *M_PI;
    return a;
}

#endif
