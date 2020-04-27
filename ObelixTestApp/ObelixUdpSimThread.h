#ifndef OBELIXUDPSIMTHREAD_H
#define OBELIXUDPSIMTHREAD_H

#include <QObject>
#include <QThread>
#include <QTimer>

#include "ObelixUdpSim.h"

class ObelixUdpSimThread : public QThread
{
public:
  ObelixUdpSimThread();


  void SetUdpReaderParameters(QString pVideoIp, uint pVideoPort, QString pTrackIp, uint pTrackPort);
  void SetVideoBeamParameters(int pNbLevel, int pNbCells);

  inline void SetTimerPeridod(int pTimerPeriod){mTimerPeriod = pTimerPeriod;}
  inline int TimerPeridod() const {return mTimerPeriod;}

    void SetSourcePostion(double pLatitude, double pLongitude);

    inline void SetAntennaRpm(uint pAntennaRpm) {mAntennaRpm = pAntennaRpm;}
    inline uint AntennaRpm() const {return mAntennaRpm;}

    inline void SetBeamWidth(double pBeamWidth) {mBeamWidth = pBeamWidth;}
    inline double BeamWidth() const {return mBeamWidth;}


    inline void SetRange(double pRange) {mRange = pRange;}
    inline double Range() const {return mRange;}

    inline void SetVideoMode(uint pVal) {mVideoMode = pVal;}
    inline uint VideoMode() const {return mVideoMode;}

    void SetVideoIntensity(double pVal);
    double VideoIntensity();


    void SetVideoNoise(double pVal);
    double VideoNoise();



    void PushTracks(T_ObelixTrack* pTrackTable, uint pCount);


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

  //
  int mBeamNbLevel;
  int mBeamNbCells;
  uint mAntennaRpm;
  double mBeamWidth;

  //
  double mAntennaPos;
  uint mVideoMode;


  QHash<uint16_t, T_ObelixTrack> mTrackTable;


  double mVideoAzGapCorrectionRto;
  double mVideoAzGapLvlRto;
  double mVideoRgGapLvlRto;
  double mVideoIntensity;
  double mVideoNoise;

  double mRange;

};

#endif // OBELIXUDPSIMTHREAD_H