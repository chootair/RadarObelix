#ifndef OBELIXUDPSIM_H
#define OBELIXUDPSIM_H

#include <math.h>

#include <QObject>
#include <QUdpSocket>
#include <QTime>

#include "ObelixLibDef.h"
#include "ObelixToolbox.h"
#include "Obelix.h"


typedef struct _T_MapObject
{
  uint8_t                 Type;
  QList<T_ObelixMapPoint> Points;
}T_MapObject;


typedef struct _T_ObelixCloudNode
{
  double Latitude;
  double Longitude;
  double Intensity;
}T_ObelixCloudNode;

typedef struct _T_ObelixCloud
{
  uint16_t Id;
  T_ObelixCloudNode Nodes[5];
}T_ObelixCloud;



class OBELIXLIBSHARED_EXPORT ObelixUdpSim : public QObject
{
  Q_OBJECT
public:
  explicit ObelixUdpSim(QObject *parent = nullptr);
  ~ObelixUdpSim();


  void SetUdpWriterVideoParameters(QString pIp, uint pPort);
  void SetUdpWriterTrackParameters(QString pIp, uint pPort);
  void SetUdpWriterMapParameters(QString pIp, uint pPort);

  //
  void SetVideoBeamParameters(int pNbLevel, int pNbCells);


  inline double VideoAzimutGapCorrectionRatio() const {return mVideoAzGapCorrectionRto;}
  inline double VideoAzimutGapLevelRatio() const {return mVideoAzGapLvlRto;}
  inline double VideoRangeGapLevelRatio() const {return mVideoRgGapLvlRto;}
  inline double VideoNoise() const {return mVideoNoise;}

  inline void SetVideoAzimutGapCorrectionRatio(double pVal) {mVideoAzGapCorrectionRto = pVal;}
  inline void SetVideoAzimutGapLevelRatio(double pVal) {mVideoAzGapLvlRto = pVal;}
  inline void SetVideoRangeGapLevelRatio(double pVal) {mVideoRgGapLvlRto = pVal;}
  inline void SetVideoNoise(double pVal) {mVideoNoise = pVal;}




  void SendVideoBeam(double pHeading, double pStartAzimut, double pAzimuthWidth, double pStartRange, double pRangeWidth, int pVideoMode);
  void SendTrackTable();
  void SendObjectMapTable();


  void SetTrackTableRef(QHash<uint16_t, T_ObelixTrack>* pTrackTable);

  void SetCloudTableRef(QHash<uint16_t, T_ObelixCloud>* pCloudTbl);

  bool PushMapObject(uint16_t pId, uint8_t pType, T_ObelixMapPoint* pPointTable, uint pCount);
  void SetPlatformPosition(double pLatitude, double pLongitude);

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
  uint16_t mMapMsgNb;

  // Video correction parameters
  double mVideoAzGapCorrectionRto;
  double mVideoAzGapLvlRto;
  double mVideoRgGapLvlRto;
  double mVideoNoise;

  // Platfrom position
  double mPtfLatitude;
  double mPtfLongitude;

  //
  QHash<uint16_t, T_ObelixTrack>* mTrackTbl;

  QHash<uint16_t, T_ObelixCloud>* mCloudTbl;




  QHash<uint16_t, T_MapObject> mMapObjectTable;


  QUdpSocket* mVideoUdpSocket;
  QUdpSocket* mTrackUdpSocket;
  QUdpSocket* mMapUdpSocket;
  //
  QHostAddress mVideoIp;
  uint    mVideoPort;
  QHostAddress mTrackIp;
  uint    mTrackPort;
  QHostAddress mMapIp;
  uint    mMapPort;

  quint32 mVideoSendErrCnt;
  quint32 mTrackSendErrCnt;
  quint32 mMapSendErrCnt;

};

#endif // OBELIXUDPSIM_H
