#ifndef OBELIXTOOLBOX_H
#define OBELIXTOOLBOX_H



#include <QtCore/QtGlobal>
#include "ObelixLibDef.h"

#define _USE_MATH_DEFINES
#include <math.h>


// ObelixToolBox namespace
namespace OTB
{


static const double  NM_TO_M        = 1852;
static const double  KNOT_TO_MS     = 0.514444;
static const double  DEG_TO_RAD     = M_PI/180;
static const double  EARTH_RADIUS_M = 6378137;


inline OBELIXLIBSHARED_EXPORT double LonToX(double lon) {return EARTH_RADIUS_M *  DEG_TO_RAD * lon;}
inline OBELIXLIBSHARED_EXPORT double LatToY(double lat) {return EARTH_RADIUS_M *  DEG_TO_RAD * lat;}
inline OBELIXLIBSHARED_EXPORT double XToLon(double x)   {return x / (EARTH_RADIUS_M *  DEG_TO_RAD);}
inline OBELIXLIBSHARED_EXPORT double YToLat(double y)   {return y / (EARTH_RADIUS_M *  DEG_TO_RAD);}



OBELIXLIBSHARED_EXPORT void ComputeAzimuthDistance(double pLatA,
                                                   double pLongA,
                                                   double pLatB,
                                                   double pLongB,
                                                   double& pAzimuth,
                                                   double& pDistance);
OBELIXLIBSHARED_EXPORT void AzDstToLatLon(double pLatA,
                                          double pLongA,
                                          double pAzimuth,
                                          double pDistance,
                                          double& pLatB,
                                          double& pLongB);


OBELIXLIBSHARED_EXPORT void ComputeTrajectory(double  pInLat,
                                              double  pInLong,
                                              double& pOutLat,
                                              double& pOutLong,
                                              double  pSpeed,
                                              double  pCourse,
                                              double  pDeltaTime);

}
#endif // OBELIXTOOLBOX_H
