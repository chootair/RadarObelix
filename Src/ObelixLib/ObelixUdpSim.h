#ifndef OBELIXUDPSIM_H
#define OBELIXUDPSIM_H

#include <QObject>
#include <QUdpSocket>

#include "ObelixLibDef.h"
#include "Obelix.h"

class OBELIXLIBSHARED_EXPORT ObelixUdpSim : public QObject
{
  Q_OBJECT
public:
  explicit ObelixUdpSim(QObject *parent = nullptr);
  ~ObelixUdpSim();


  void SetUdpReaderVideoParameters(QString pIp, uint pPort);
  void SetUdpReaderTrackParameters(QString pIp, uint pPort);
  void SetVideoBeamParameters(int pNbLevel, int pNbCells);


   inline double VideoAzimutGapCorrectionRatio() const {return mVideoAzGapCorrectionRto;}
   inline double VideoAzimutGapLevelRatio() const {return mVideoAzGapLvlRto;}
   inline double VideoRangeGapLevelRatio() const {return mVideoRgGapLvlRto;}
   inline double VideoIntensity() const {return mVideoIntensity;}
   inline double VideoNoise() const {return mVideoNoise;}

   inline void SetVideoAzimutGapCorrectionRatio(double pVal) {mVideoAzGapCorrectionRto = pVal;}
   inline void SetVideoAzimutGapLevelRatio(double pVal) {mVideoAzGapLvlRto = pVal;}
   inline void SetVideoRangeGapLevelRatio(double pVal) {mVideoRgGapLvlRto = pVal;}
   inline void SetVideoIntensity(double pVal) {mVideoIntensity = pVal;}
   inline void SetVideoNoise(double pVal) {mVideoNoise = pVal;}




  void SendVideoBeam(double pHeading, double pStartAzimut, double pAzimuthWidth, double pStartRange, double pRangeWidth, int pVideoMode);
  void SendTrackTable();


  void SetTrackTableRef(QHash<uint16_t, T_ObelixTrack>* pTrackTable);

signals:

public slots:


private:

 void BuildVideoBeam(double pHeading, double pStartAzimut, double pAzimuthWidth, double pStartRange, double pRangeWidth, int pVideoMode);

private:
  // Video beam parameters
  int mBeamNbLevel;
  double mBeamWidth;
  double mRangeNm;
  int mBeamNbCells;

  // Beam definition
  T_ObelixVideoMessage* mBeam;
  int                   mBeamSize;

  uint16_t mVideoMsgNb;
  uint16_t mTrackMsgNb;

  // Video correction parameters
  double mVideoAzGapCorrectionRto;
  double mVideoAzGapLvlRto;
  double mVideoRgGapLvlRto;
  double mVideoIntensity;
  double mVideoNoise;

  //
  QHash<uint16_t, T_ObelixTrack>* mTrackTbl;


  QUdpSocket* mVideoUdpSocket;
  QUdpSocket* mTrackUdpSocket;
  //
  QHostAddress mVideoIp;
  uint    mVideoPort;
  QHostAddress mTrackIp;
  uint    mTrackPort;

  quint32 mVideoSendErrCnt;
  quint32 mTrackSendErrCnt;

};

#endif // OBELIXUDPSIM_H
