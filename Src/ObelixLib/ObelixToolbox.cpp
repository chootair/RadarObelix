

#include "ObelixToolbox.h"




void OTB::ComputeAzimuthDistance(double pLatA,
                                 double pLongA,
                                 double pLatB,
                                 double pLongB,
                                 double& pAzimuth,
                                 double& pDistance)
{
  double lXA = LonToX(pLongA);
  double lYA = LatToY(pLatA);
  //
  double lXB = LonToX(pLongB);
  double lYB = LatToY(pLatB);

  //
  pDistance = sqrt((lYB-lYA)*(lYB-lYA) + (lXB-lXA)*(lXB-lXA));

  /// \todo Improuve
  if (fabs(lYB-lYA) < 1)
  {
    pAzimuth = 0;
  }
  else if (fabs(lYB-lYA) < 1)
  {
    pAzimuth = 0;
  }

  pAzimuth = atan2((lXB-lXA),(lYB-lYA))/DEG_TO_RAD;


  // Negative correction
  if (pAzimuth < 0)
  {
    pAzimuth = 360 + pAzimuth;
  }
}


void OTB::AzDstToLatLon(double pLatA,
                        double pLongA,
                        double pAzimuth,
                        double pDistance,
                        double &pLatB,
                        double &pLongB)
{
  double lXA = LonToX(pLongA);
  double lYA = LatToY(pLatA);
  //
  double lXB = 0;
  double lYB = 0;

  lXB = lXA + pDistance * sin(pAzimuth*DEG_TO_RAD);
  lYB = lYA + pDistance * cos(pAzimuth*DEG_TO_RAD);

  pLongB = XToLon(lXB);
  pLatB  = YToLat(lYB);
}

void OTB::ComputeTrajectory(double pInLat, double pInLong, double &pOutLat, double &pOutLong, double pSpeed, double pCourse, double pDeltaTime)
{
  pOutLong  = XToLon(pDeltaTime * pSpeed * sin(pCourse*DEG_TO_RAD) * KNOT_TO_MS + LonToX(pInLong));
  pOutLat   = YToLat(pDeltaTime * pSpeed * cos(pCourse*DEG_TO_RAD) * KNOT_TO_MS + LatToY(pInLat));
}
