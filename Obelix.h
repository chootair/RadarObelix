#ifndef OBELIX_H
#define OBELIX_H

#include <cstdint>

static const uint16_t OBELIX_CELL_COUNT = 200;

typedef struct _T_ObelixVideoMessage
{
  uint16_t Number;
  uint16_t CellCount;
  double   StartAzDeg;
  double   EndAzDeg;
  double   StartRgNm;
  double   CellWidthNm;
  uint8_t  CellValueTbl[OBELIX_CELL_COUNT];
}T_ObelixVideoMessage;



static const double OBELIX_TRACK_LATLONG_LSB       = 180.0 / (((uint64_t)1<<31)-1);
static const double OBELIX_TRACK_BEARINGCOURSE_LSB = 360.0 / (((uint32_t)1<<16)-1);
static const double OBELIX_TRACK_DISTANCE_LSB      = 0.1;
static const double OBELIX_TRACK_SPEED_LSB         = 0.1;
static const uint16_t OBELIX_TRACK_TABLE_COUNT = 20;


typedef struct _T_ObelixTrack
{
  uint16_t Id;
  uint8_t  Status;
  uint8_t  Spare;
  int32_t  LatitudeDeg;
  int32_t  LongitudeDeg;
  uint16_t BearingDeg;
  uint16_t DistanceNm;
  uint16_t CourseDeg;
  uint16_t GroundSpeedKts;
  uint16_t FlightLevel;
  char     CallSing[6];
}T_ObelixTrack;

/// \note Obelix track = 28 Bytes


typedef struct _T_ObelixTrackReportMessage
{
  uint16_t       Number;
  uint16_t       TrackCount;
  T_ObelixTrack  TrackTbl[OBELIX_TRACK_TABLE_COUNT];
}T_ObelixTrackReportMessage;



#endif // OBELIX_H
