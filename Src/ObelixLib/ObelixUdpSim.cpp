#include "ObelixUdpSim.h"

ObelixUdpSim::ObelixUdpSim(QObject *parent) : QObject(parent)
{


  mVideoMsgNb = 0;
  mTrackMsgNb = 0;

  //
  mVideoAzGapCorrectionRto = 1;
  mVideoAzGapLvlRto        = 2;
  mVideoRgGapLvlRto        = 100;
  mVideoIntensity          = 1;
  mVideoNoise              = 0;


  //
  mTrackTbl = nullptr;


  //
  mVideoUdpSocket = new QUdpSocket(this);
  mTrackUdpSocket = new QUdpSocket(this);
  //
  mVideoIp    = QHostAddress::Null;
  mVideoPort = 0;
  mTrackIp   = QHostAddress::Null;
  mTrackPort = 0;
  //
  mVideoSendErrCnt = 0;
  mTrackSendErrCnt = 0;

  mBeam = nullptr;
  mBeamSize = 0;
}

ObelixUdpSim::~ObelixUdpSim()
{
  delete mTrackUdpSocket;
  delete mVideoUdpSocket;
}

void ObelixUdpSim::SetUdpReaderVideoParameters(QString pIp, uint pPort)
{
  mVideoIp = QHostAddress(pIp);
  mVideoPort = pPort;
}

void ObelixUdpSim::SetUdpReaderTrackParameters(QString pIp, uint pPort)
{
  mTrackIp = QHostAddress(pIp);
  mTrackPort = pPort;
}

void ObelixUdpSim::SetVideoBeamParameters(int pNbLevel, int pNbCells)
{
  if (mBeam != nullptr)
  {
    free (mBeam);
    mBeam = nullptr;
  }

  //
  mBeamNbCells = pNbCells;
  mBeamNbLevel = pNbLevel;
  mBeamSize    = (1<<mBeamNbLevel) -1;

  //
  mBeam = (T_ObelixVideoMessage*)calloc(mBeamSize, sizeof(T_ObelixVideoMessage));

  //
  for (int lIdxBeam=0; lIdxBeam<mBeamSize; lIdxBeam++)
  {
    mBeam[lIdxBeam].StartRgNm   = 0;
    mBeam[lIdxBeam].CellWidthNm = 0;
    mBeam[lIdxBeam].CellCount   = mBeamNbCells/mBeamNbLevel;
    mBeam[lIdxBeam].CellCount   = qMin(mBeam[lIdxBeam].CellCount, OBX_CELL_TBL_CNT);
  }
}

void ObelixUdpSim::SendVideoBeam(double pHeading, double pStartAzimut, double pAzimuthWidth, double pStartRange, double pRangeWidth, int pVideoMode)
{
  qint64 lWriteSz=0;

  // Build
  BuildVideoBeam(pHeading, pStartAzimut, pAzimuthWidth, pStartRange, pRangeWidth, pVideoMode);

  // Send beam packets
  for (int lIdxBeam=0; lIdxBeam<mBeamSize; lIdxBeam++)
  {
    lWriteSz = mVideoUdpSocket->writeDatagram((char*)&(mBeam[lIdxBeam]),sizeof(T_ObelixVideoMessage),mVideoIp,mVideoPort);
    if (lWriteSz != sizeof(T_ObelixVideoMessage))
    {
      mVideoSendErrCnt++;
    }
  }
}

void ObelixUdpSim::SendTrackTable()
{
  T_ObelixTrackReportMessage lTrackReport;
  qint64 lWriteSz=0;
  int i=0;
  QHash<uint16_t, T_ObelixTrack>::iterator lTrackIter;

  // Init
  lTrackReport.Number     = mTrackMsgNb;
  lTrackReport.TrackCount = 0;

  // Loop on the track table
  for (lTrackIter = mTrackTbl->begin(); lTrackIter != mTrackTbl->end(); ++lTrackIter)
  {
    // Add track to reprort message
     lTrackReport.TrackTbl[lTrackReport.TrackCount] = lTrackIter.value();
     lTrackReport.TrackCount++;

     // Message full or end of table?
     if ((lTrackReport.TrackCount >= OBX_TRK_TBL_CNT) || (lTrackIter == mTrackTbl->end()))
     {
       // Write message datagram
       lWriteSz = mTrackUdpSocket->writeDatagram((char*)&(lTrackReport),sizeof(T_ObelixTrackReportMessage), mTrackIp, mTrackPort);
       if (lWriteSz != sizeof(T_ObelixTrackReportMessage))
       {
         mTrackSendErrCnt++;
       }

       // Reset
       mTrackMsgNb++;
       lTrackReport.Number     = mTrackMsgNb;
       lTrackReport.TrackCount = 0;
     }
  }
}

void ObelixUdpSim::SetTrackTableRef(QHash<uint16_t, T_ObelixTrack> *pTrackTable)
{
  mTrackTbl = pTrackTable;
}


void ObelixUdpSim::BuildVideoBeam(double pHeading, double pStartAzimut, double pAzimuthWidth, double pStartRange, double pRangeWidth, int pVideoMode)
{
   QHash<uint16_t, T_ObelixTrack>::iterator lTrackIter;
  int lIdxBeam = 0;

  // Loop on beam levels
  for (int lIdxLvl=0; lIdxLvl<mBeamNbLevel; lIdxLvl++)
  {
    // Number of sub-beam for this level
    int lLevelSubBeamCnt = 1<<lIdxLvl;

    // Sub-beam width
    double lSubBeamAzWidthDeg = pAzimuthWidth/lLevelSubBeamCnt;
    double lSubBeamRgWidthNm  = pRangeWidth/mBeamNbCells;

    // Loop on the sub-beams of the level
    for (int lIdxSubBeam=0; lIdxSubBeam<lLevelSubBeamCnt; lIdxSubBeam++)
    {
      // Message counter
      mBeam[lIdxBeam].Number = mVideoMsgNb;
      mVideoMsgNb++;

      // Platform heading
      mBeam[lIdxBeam].HeadingDeg = pHeading;

      // Sub-Beam start and stop azimut
      mBeam[lIdxBeam].StartAzDeg = pStartAzimut + lIdxSubBeam*lSubBeamAzWidthDeg;
      mBeam[lIdxBeam].EndAzDeg   = mBeam[lIdxBeam].StartAzDeg + lSubBeamAzWidthDeg;

      // Sub-Beam start range
      mBeam[lIdxBeam].CellWidthNm = lSubBeamRgWidthNm;
      mBeam[lIdxBeam].StartRgNm   = pStartRange + lIdxLvl * mBeam[lIdxBeam].CellWidthNm * mBeam[lIdxBeam].CellCount;


      mBeam[lIdxBeam].VideoMode = static_cast<uint8_t>(pVideoMode);

      // Sub-Beam cells value
      for (int lIdxCell=0; lIdxCell<mBeam[lIdxLvl].CellCount; lIdxCell++)
      {
        // Reset
        mBeam[lIdxBeam].CellValueTbl[lIdxCell]=0;

        // Cell range ratio
        double lCellRgRto = (mBeam[lIdxBeam].StartRgNm+(lIdxCell+0.5)*mBeam[lIdxBeam].CellWidthNm-pStartRange)/pRangeWidth;

        // Search
        if (pVideoMode == OBX_VIDEO_SEARCH)
        {
          // Noise level
          mBeam[lIdxBeam].CellValueTbl[lIdxCell] = mVideoNoise*static_cast<double>(rand()%255);

          // Loop on track table
          for (lTrackIter = mTrackTbl->begin(); lTrackIter != mTrackTbl->end(); ++lTrackIter)
          {
            // Valid track
            /// \todo Manage valid track

            // Track gap
            double lAzGapDeg = fabs((mBeam[lIdxBeam].StartAzDeg + 0.5 *lSubBeamAzWidthDeg) - lTrackIter.value().Bearing*OBX_TRK_BEARINGCOURSE_LSB);
            double lRgGapNm  = fabs(mBeam[lIdxBeam].StartRgNm+(lIdxCell+0.5)*mBeam[lIdxBeam].CellWidthNm - lTrackIter.value().Distance*OBX_TRK_DISTANCE_LSB);

            // Gap correction
            lAzGapDeg = lAzGapDeg*(1-(1-lCellRgRto)*mVideoAzGapCorrectionRto);
            lRgGapNm  = lRgGapNm/pRangeWidth;

            // Intens
            double lAzLvl = qBound(0.0, 1 - mVideoAzGapLvlRto * lAzGapDeg, 1.0);
            double lRgLvl = qBound(0.0, 1 - mVideoRgGapLvlRto * lRgGapNm, 1.0);
            int    lLvl = 255 * mVideoIntensity * lAzLvl * lRgLvl;

            //
            mBeam[lIdxBeam].CellValueTbl[lIdxCell]= qMin(255, mBeam[lIdxBeam].CellValueTbl[lIdxCell]+lLvl);
          }
        }
        // Meteo mode
        else if (pVideoMode == OBX_VIDEO_WEATHER)
        {
          /// \todo
          mBeam[lIdxBeam].CellValueTbl[lIdxCell]=rand()%128;
        }
        // Random mode
        else if (pVideoMode == OBX_VIDEO_TEST)
        {
          mBeam[lIdxBeam].CellValueTbl[lIdxCell]=rand()%255;
        }
      }

      //
      lIdxBeam++;
    }
  }
}
