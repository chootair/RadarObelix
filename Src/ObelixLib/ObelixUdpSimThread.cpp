#include "ObelixUdpSimThread.h"

ObelixUdpSimThread::ObelixUdpSimThread()
{
  mRange      = 100;
  mAntennaPos = 0;
  mAntennaRpm = 10;
  mBeamWidth  = 1;
  mTimerPeriod = 50;
  mVideoAzGapCorrectionRto = 1;
  mVideoAzGapLvlRto        = 2;
  mVideoRgGapLvlRto        = 100;
  mVideoIntensity          = 1;
  mVideoNoise              = 0;
  mVideoMode = OBX_VIDEO_SEARCH;

  //
  mObelixUdpGene = nullptr;
}

void ObelixUdpSimThread::SetUdpReaderParameters(QString pVideoIp, uint pVideoPort, QString pTrackIp, uint pTrackPort)
{
  mVideoIp = pVideoIp;
  mVideoPort = pVideoPort;
  mTrackIp = pTrackIp;
  mTrackPort = pTrackPort;
}

void ObelixUdpSimThread::SetVideoBeamParameters(int pNbLevel, int pNbCells)
{
  mBeamNbLevel = pNbLevel;
  mBeamNbCells = pNbCells;
}

void ObelixUdpSimThread::SetVideoIntensity(double pVal)
{
  if (mObelixUdpGene)
  {
    mObelixUdpGene->SetVideoIntensity(pVal);
  }
  mVideoIntensity = pVal;
}

double ObelixUdpSimThread::VideoIntensity()
{
  if (mObelixUdpGene)
  {
    mVideoIntensity = mObelixUdpGene->VideoIntensity();
  }

  return mVideoIntensity;
}

void ObelixUdpSimThread::SetVideoNoise(double pVal)
{
  if (mObelixUdpGene)
  {
    mObelixUdpGene->SetVideoNoise(pVal);
  }
  mVideoNoise = pVal;
}

double ObelixUdpSimThread::VideoNoise()
{
  if (mObelixUdpGene)
  {
    mVideoNoise = mObelixUdpGene->VideoNoise();
  }

  return mVideoNoise;
}

void ObelixUdpSimThread::PushTracks(T_ObelixTrack *pTrackTable, uint pCount)
{
  // Loop on tracks
  for (uint i=0; i<pCount; i++)
  {
    // Update
    if (mTrackTable.contains(pTrackTable[i].Id) == true)
    {
      mTrackTable[pTrackTable[i].Id] = pTrackTable[i];
    }
    // Add
    else
    {
      mTrackTable.insert(pTrackTable[i].Id, pTrackTable[i]);
    }
  }

  //qDebug("TableCnt=%i", mTrackTable.count());
/*
  QHash<uint16_t, T_ObelixTrack>::iterator lTrackIter;
  int i=0;
  for (lTrackIter = mTrackTable.begin(); lTrackIter != mTrackTable.end(); ++lTrackIter)
  {
    qDebug("[%i]Id:%i", i,lTrackIter.value().Id);
    i++;
  }*/
}

void ObelixUdpSimThread::AskForStop()
{
  mRun = false;
  exit();
  delete mObelixUdpGene;
  mObelixUdpGene = nullptr;
}


void ObelixUdpSimThread::run()
{


  qDebug("Run is executed in thread : %X", currentThreadId());




  mObelixUdpGene = new ObelixUdpSim();


  mObelixUdpGene->SetUdpReaderVideoParameters(mVideoIp, mVideoPort);
  mObelixUdpGene->SetUdpReaderTrackParameters(mTrackIp, mTrackPort);
  mObelixUdpGene->SetVideoBeamParameters(mBeamNbLevel, mBeamNbCells);
  mObelixUdpGene->SetTrackTableRef(&mTrackTable);



  mObelixUdpGene->SetVideoIntensity(mVideoIntensity);
   mObelixUdpGene->SetVideoNoise(mVideoNoise);


  mObelixUdpGene->moveToThread(this);

  mTimer = new QTimer();
  mTimer->moveToThread(this);
  connect(mTimer , &QTimer::timeout, this, &ObelixUdpSimThread::OnTimerTick, Qt::DirectConnection);
  mTimer->start(mTimerPeriod);
  exec();

}

void ObelixUdpSimThread::OnTimerTick()
{
  // Video beams

  //
  double lTickBeamWidth  = static_cast<double>(mTimerPeriod) * 6 * mAntennaRpm/1000.0;
  uint lTickBeamRepeat =  static_cast<uint>(ceil(lTickBeamWidth/mBeamWidth));

  // Beam repetition
  for (uint lIdxBeam=0; lIdxBeam<lTickBeamRepeat; lIdxBeam++)
  {
    // Send video beam
    mObelixUdpGene->SendVideoBeam(mPlatformHeading, mAntennaPos,mBeamWidth,0,mRange,mVideoMode);

    // Update antenna position
    mAntennaPos += mBeamWidth;

    // Lap correction
    if (mAntennaPos >= 360)
    {
      mAntennaPos = mAntennaPos-360;
    }
  }

  /// \todo Do less
  // Send Track report
  mObelixUdpGene->SendTrackTable();
}
