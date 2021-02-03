//----------------------------------------------------------------------------//
//--                              Radar OBELIX                              --//
//----------------------------------------------------------------------------//
/// \file    Oblelix.h
/// \brief   General definitions
/// ---------------------------------------------------------------------------
/// \version 1.0
/// \author  Guillaume.C
/// \date    01/04/2020
/// \note    Creation for initial version
//----------------------------------------------------------------------------//
#ifndef OBELIX_H
#define OBELIX_H

#include <cstdint>

static const uint16_t OBX_CELL_TBL_CNT = 200;

/// \defgroup gVideoMode Video Mode
static const uint16_t OBX_VIDEO_SEARCH  = 0;
static const uint16_t OBX_VIDEO_WEATHER = 1;
static const uint16_t OBX_VIDEO_TEST    = 2;


/// \brief Obelix video message
typedef struct _T_ObelixVideoMessage
{
  uint16_t Number;                         ///< Message number
  uint16_t CellCount;                      ///< Cell count in the message
  double   HeadingDeg;                     ///< Platform heading [°]
  double   StartAzDeg;                     ///< Start azimuth    [°] (North reference)
  double   EndAzDeg;                       ///< End azimuth      [°] (North reference)
  double   StartRgNm;                      ///< Start range      [Nm]
  double   CellWidthNm;                    ///< Cell width       [Nm]
  uint8_t  VideoMode;                      ///< Video mode \sa gVideoMode
  uint8_t  CellValueTbl[OBX_CELL_TBL_CNT]; ///< Cell table values
}T_ObelixVideoMessage;



static const double OBX_TRK_LATLONG_LSB       = 180.0 / (((uint64_t)1<<31)-1);
static const double OBX_TRK_BEARINGCOURSE_LSB = 360.0 / (((uint32_t)1<<16)-1);
static const double OBX_TRK_DISTANCE_LSB      = 0.1;
static const double OBX_TRK_SPEED_LSB         = 0.1;
static const double OBX_TRK_ALT_LSB           = 25;
static const uint16_t OBX_TRK_TBL_CNT         = 10;

/// \brief Obelix track
typedef struct _T_ObelixTrack
{
  uint16_t Id;
  uint8_t  Status;
  uint8_t  Quality;
  // Position
  int32_t  Latitude;  ///< Latitude  [°]
  int32_t  Longitude; ///< Longitude [°]
  uint16_t Altitude;  ///< Altitude  [ft]
  uint16_t Bearing;   ///< Bearing   [°]  (from platform)
  uint16_t Distance;  ///< Bearing   [Nm] (from platform)
  // Kinematics
  uint16_t Course;      ///< Course       [°]
  uint16_t GroundSpeed; ///< Ground speed [kts]
  // Transponder
  uint16_t	Mode1Code;
  uint16_t	Mode2Code;
  uint16_t	Mode3ACode; ///< Mode A Squawk code
  uint16_t	ModeCCode;  ///< Mode C Altitude x100Ft
  uint16_t	ModeSCode;
  uint32_t  AircraftId; ///<
  // Callsing
  char     CallSing[8]; ///< Track callsing
  //
  int32_t  TimeOfDay;
}T_ObelixTrack;
// Size 42

/// \brief Obelix track message
typedef struct _T_ObelixTrackReportMessage
{
  uint16_t       Number;                    ///< Message number
  uint16_t       TrackCount;                ///< Number of track in the message
  T_ObelixTrack  TrackTbl[OBX_TRK_TBL_CNT]; ///< Track table
}T_ObelixTrackReportMessage;



#endif // OBELIX_H
