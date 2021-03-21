#ifndef OBELIXUDPSIMTHREAD_H
#define OBELIXUDPSIMTHREAD_H

#include <QObject>
#include <QThread>
#include <QTimer>

#include "ObelixUdpSim.h"

class OBELIXLIBSHARED_EXPORT ObelixUdpSimThread : public QThread
{
public:
  ObelixUdpSimThread();


  void SetUdpReaderParameters(QString pVideoIp, uint pVideoPort,
                              QString pTrackIp, uint pTrackPort,
                              QString pMapIp  , uint pMapPort);
  void SetVideoBeamParameters(int pNbLevel, int pNbCells);

  inline void SetTimerPeridod(int pTimerPeriod){mTimerPeriod = pTimerPeriod;}
  inline int TimerPeridod() const {return mTimerPeriod;}

  void SetSourcePostion(double pLatitude, double pLongitude);

  inline void SetAntennaRpm(uint pAntennaRpm) {mAntennaRpm = pAntennaRpm;}
  inline uint AntennaRpm() const {return mAntennaRpm;}

  inline void SetBeamWidth(double pBeamWidth) {mBeamWidth = pBeamWidth;}
  inline double BeamWidth() const {return mBeamWidth;}



  inline bool   SectorScan           () const{return mSectorScan;}
  inline bool   SectorScanPlatformRef() const{return mSectorPlatfromRef;}
  inline double SectorScanAzimuth     () const{return mSectorScanAzimuthDeg;}
  inline double SectorScanWidth      () const{return mSectorScanWidthDeg;}

  inline void SetSectorScan           (bool   pSectorScan)          {mSectorScan = pSectorScan;}
  inline void SetSectorScanPlatformRef(bool   pSectorPlatfromRef)   {mSectorPlatfromRef = pSectorPlatfromRef;}
  inline void SetSectorScanAzimuth     (double pSectorScanAzimuthDeg) {mSectorScanAzimuthDeg = pSectorScanAzimuthDeg;}
  inline void SetSectorScanWidth      (double pSectorScanWidthDeg)  {mSectorScanWidthDeg = pSectorScanWidthDeg;}



  void SetPlatformPosition(double pLatitude, double pLongitude);

  inline void SetRange(double pRange) {mRange = pRange;}
  inline double Range() const {return mRange;}

  inline void SetVideoMode(uint pVal) {mVideoMode = pVal;}
  inline uint VideoMode() const {return mVideoMode;}



  inline void SetPlatformHeading(double_t pVal) {mPlatformHeading = pVal;}
  inline double PlatformHeading() const {return mPlatformHeading;}

  inline void SetAntennaInverseRotate(bool pVal) {mAntennaInverseRotate = pVal;}
  inline bool AntennaInverseRotate() const {return mAntennaInverseRotate;}




  void SetVideoNoise(double pVal);
  double VideoNoise();

  void SetVideoAzimutGapCorrectionRatio(double pVal);
  double VideoAzimutGapCorrectionRatio();

  void SetVideoAzimutGapLevelRatio(double pVal);
  double VideoAzimutGapLevelRatio();

  void SetVideoRangeGapLevelRatio(double pVal);
  double VideoRangeGapLevelRatio();







  void RemoveTrack( uint16_t pTrackId);
  void PushTrack(T_ObelixTrack pTrack);
  void PushTracks(T_ObelixTrack* pTrackTable, uint pCount);
  void PushClouds(T_ObelixCloud *pCloudTable, uint pCount);

  bool PushMapObject(uint16_t pId, uint8_t pType, T_ObelixMapPoint* pPointTable, uint pCount);



  void PushMapPlolygon(T_ObelixMapPoint* pPointTable, uint pCount);
  void PushMapPoints(T_ObelixMapPoint* pPointTable, uint pCount);
  void PushMapPattern(T_ObelixMapPoint* pPointTable, uint pCount);



  void AskForStop();

private:
  void run();

private slots:
  void OnTimerTick();

private:
  bool mRun;

  ObelixUdpSim* mObelixUdpGene;
  QTimer* mTimer;
  int mTimerPeriod;

  //
  QString mVideoIp;
  uint    mVideoPort;
  QString mTrackIp;
  uint    mTrackPort;
  QString mMapIp;
  uint    mMapPort;
  //
  int mBeamNbLevel;
  int mBeamNbCells;
  uint mAntennaRpm;
  double mBeamWidth;

  //
  double mAntennaPos;
  uint mVideoMode;



  QHash<uint16_t, T_ObelixTrack> mTrackTable;

  QHash<uint16_t, T_ObelixCloud> mCloudTable;





  double mVideoAzGapCorrectionRto;
  double mVideoAzGapLvlRto;
  double mVideoRgGapLvlRto;
  double mVideoIntensity;
  double mVideoNoise;

  double mRange;


  double mPlatformHeading;

  bool mAntennaInverseRotate;
  bool mAntennaInverseCurrentRotate;

  bool mSectorScan;
  bool mSectorPlatfromRef;
  double mSectorScanAzimuthDeg;
  double mSectorScanWidthDeg;


};

#endif // OBELIXUDPSIMTHREAD_H
