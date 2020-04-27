#ifndef OBELIX_H
#define OBELIX_H

#include <cstdint>

static const uint16_t OBX_CELL_TBL_CNT = 200;

static const uint16_t OBX_VIDEO_SEARCH  = 0;
static const uint16_t OBX_VIDEO_WEATHER = 1;
static const uint16_t OBX_VIDEO_TEST    = 2;

typedef struct _T_ObelixVideoMessage
{
  uint16_t Number;
  uint16_t CellCount;
  double   StartAzDeg;
  double   EndAzDeg;
  double   StartRgNm;
  double   CellWidthNm;
  uint8_t  VideoMode;
  uint8_t  CellValueTbl[OBX_CELL_TBL_CNT];
}T_ObelixVideoMessage;



static const double OBX_TRK_LATLONG_LSB       = 180.0 / (((uint64_t)1<<31)-1);
static const double OBX_TRK_BEARINGCOURSE_LSB = 360.0 / (((uint32_t)1<<16)-1);
static const double OBX_TRK_DISTANCE_LSB      = 0.1;
static const double OBX_TRK_SPEED_LSB         = 0.1;
static const double OBX_TRK_ALT_LSB           = 25;
static const uint16_t OBX_TRK_TBL_CNT         = 10;


typedef struct _T_ObelixTrack
{
  uint16_t Id;
  uint8_t  Status;
  uint8_t  Quality;
  // Position
  int32_t  Latitude;  ///< Latitude in deg
  int32_t  Longitude;
  uint16_t Altitude;
  uint16_t Bearing;
  uint16_t Distance;
  // Kinematics
  uint16_t Course;
  uint16_t GroundSpeed;
  // Transponder
  uint16_t	Mode1Code;
  uint16_t	Mode2Code;
  uint16_t	Mode3ACode; ///< Mode A Squawk code
  uint16_t	ModeCCode;  ///< Mode C Altitude x100Ft
  uint16_t	ModeSCode;
  uint32_t  AircraftId;
  // Callsing
  char     CallSing[8];
  //
  int32_t  TimeOfDay;
}T_ObelixTrack;
// Size 42




typedef struct _T_ObelixTrackReportMessage
{
  uint16_t       Number;
  uint16_t       TrackCount;
  T_ObelixTrack  TrackTbl[OBX_TRK_TBL_CNT];
}T_ObelixTrackReportMessage;



#endif // OBELIX_H
