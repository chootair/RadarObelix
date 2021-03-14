#include "ObelixUdpSim.h"

ObelixUdpSim::ObelixUdpSim(QObject *parent) : QObject(parent)
{


  mVideoMsgNb = 0;
  mTrackMsgNb = 0;
  mMapMsgNb   = 0;

  //
  mVideoAzGapCorrectionRto = 1;
  mVideoAzGapLvlRto        = 2;
  mVideoRgGapLvlRto        = 100;
  mVideoIntensity          = 1;
  mVideoNoise              = 0;


  //
  mTrackTbl = nullptr;
  mCloudTbl = nullptr;

  //
  mPtfLatitude  = 0;
  mPtfLongitude = 0;


  //
  mVideoUdpSocket = new QUdpSocket(this);
  mTrackUdpSocket = new QUdpSocket(this);
  mMapUdpSocket   = new QUdpSocket(this);
  //
  mVideoIp    = QHostAddress::Null;
  mVideoPort = 0;
  mTrackIp   = QHostAddress::Null;
  mTrackPort = 0;
  //
  mVideoSendErrCnt = 0;
  mTrackSendErrCnt = 0;
  mMapSendErrCnt   = 0;

  mBeam = nullptr;
  mBeamSize = 0;
}

ObelixUdpSim::~ObelixUdpSim()
{
  delete mMapUdpSocket;
  delete mTrackUdpSocket;
  delete mVideoUdpSocket;
}

void ObelixUdpSim::SetUdpWriterVideoParameters(QString pIp, uint pPort)
{
  mVideoIp = QHostAddress(pIp);
  mVideoPort = pPort;
}

void ObelixUdpSim::SetUdpWriterTrackParameters(QString pIp, uint pPort)
{
  mTrackIp = QHostAddress(pIp);
  mTrackPort = pPort;
}

void ObelixUdpSim::SetUdpWriterMapParameters(QString pIp, uint pPort)
{
  mMapIp = QHostAddress(pIp);
  mMapPort = pPort;
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

void ObelixUdpSim::SendObjectMapTable()
{
  T_ObelixMapMessage lMapMessage;
  qint64 lWriteSz=0;
  int i=0;
  QHash<uint16_t, T_MapObject>::iterator lObjectIter;

  // Loop on the map table
  for (lObjectIter = mMapObjectTable.begin(); lObjectIter != mMapObjectTable.end(); ++lObjectIter)
  {
    // Header
    lMapMessage.Number        = mMapMsgNb;
    lMapMessage.OperationType = OBX_MAP_UPDATE;

    // Platform
    lMapMessage.PlatformLatitude  = mPtfLatitude/OBX_TRK_LATLONG_LSB;
    lMapMessage.PlatformLongitude = mPtfLongitude/OBX_TRK_LATLONG_LSB;

    // Map object
    lMapMessage.ElementId   = lObjectIter.key();
    lMapMessage.ElementType = lObjectIter.value().Type;
    lMapMessage.PointCount  = qMin(lObjectIter.value().Points.count(), (int)OBX_MAP_TBL_CNT); /// \todo Mutiple message for large map object
    //
    for (uint16_t i=0; i<lMapMessage.PointCount; i++)
    {
      lMapMessage.PointTbl[i] = lObjectIter.value().Points.at(i);
    }

    // Write message datagram
    lWriteSz = mMapUdpSocket->writeDatagram((char*)&(lMapMessage),sizeof(T_ObelixMapMessage), mMapIp, mMapPort);
    if (lWriteSz != sizeof(T_ObelixTrackReportMessage))
    {
      mMapSendErrCnt++;
    }

    // Reset
    mMapMsgNb++;
  }
}

void ObelixUdpSim::SetTrackTableRef(QHash<uint16_t, T_ObelixTrack> *pTrackTable)
{
  mTrackTbl = pTrackTable;
}

void ObelixUdpSim::SetCloudTableRef(QHash<uint16_t, T_ObelixCloud> *pCloudTbl)
{
  mCloudTbl = pCloudTbl;
}

bool ObelixUdpSim::PushMapObject(uint16_t pId, uint8_t pType, T_ObelixMapPoint *pPointTable, uint pCount)
{
  // New map object
  T_MapObject newMapObject;
  newMapObject.Type = pType;
  for(uint i=0; i<pCount; i++)
  {
    newMapObject.Points.append(pPointTable[i]);
  }

  // Add or update
  if (false == mMapObjectTable.contains(pId))
  {
    mMapObjectTable.insert(pId, newMapObject);
  }
  else
  {
    mMapObjectTable[pId] = newMapObject;
  }

  //
  return true;
}

void ObelixUdpSim::SetPlatformPosition(double pLatitude, double pLongitude)
{
  mPtfLatitude  = pLatitude;
  mPtfLongitude = pLongitude;
}


void ObelixUdpSim::BuildVideoBeam(double pHeading, double pStartAzimut, double pAzimuthWidth, double pStartRange, double pRangeWidth, int pVideoMode)
{
   QHash<uint16_t, T_ObelixTrack>::iterator lTrackIter;
   QHash<uint16_t, T_ObelixCloud>::iterator lCloudIter;
   QTime lNow     = QTime::currentTime();
   int   lIdxBeam = 0;

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

        // Cell postion R/Theta
        double lCellAzDeg = mBeam[lIdxBeam].StartAzDeg + 0.5 *lSubBeamAzWidthDeg;
        double lCellRgNm  = mBeam[lIdxBeam].StartRgNm+(lIdxCell+0.5)*mBeam[lIdxBeam].CellWidthNm;

        // Cell position Lat/Long
        double lCellLat = 0;
        double lCellLon = 0;
        OTB::AzDstToLatLon(mPtfLatitude, mPtfLongitude, lCellAzDeg, lCellRgNm*OTB::NM_TO_M, lCellLat, lCellLon);


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
          // Loop on coulds
          for (lCloudIter = mCloudTbl->begin(); lCloudIter != mCloudTbl->end(); ++lCloudIter)
          {
            // Loop on nodes
            for (int j=0; j<5; j++)
            {
              // Distance of the cell to the cloud
              double lNodeAzimuth  = 0;
              double lNodeDistance = 0;
              OTB::ComputeAzimuthDistance(lCellLat, lCellLon, lCloudIter.value().Nodes[j].Latitude, lCloudIter.value().Nodes[j].Longitude,lNodeAzimuth,lNodeDistance);


             lNodeDistance = (lNodeDistance / OTB::NM_TO_M);

            //int    lLvl = 255 * 0.33 * lCloudIter.value().Nodes[j].Intensity/lNodeDistance;
            double    lLvl = 100.0 * qMax(0.0, (30.0-lNodeDistance)/30.0)*lCloudIter.value().Nodes[j].Intensity;
            int  lILvl = lLvl;

            //qDebug("------[%i][%i]%f-%f",lCloudIter.key(), j, lCellLat, lCellLon);
            //qDebug("------[%i][%i]%f-%f",lCloudIter.key(), j, lCloudIter.value().Nodes[j].Latitude, lCloudIter.value().Nodes[j].Longitude);
            //qDebug("------[%i]%f-%f",j,lNodeAzimuth,lNodeDistance);
            //qDebug("------");

            //
            mBeam[lIdxBeam].CellValueTbl[lIdxCell]= qMin(100, mBeam[lIdxBeam].CellValueTbl[lIdxCell]+lILvl);
            }
          }

          /// \todo
          ///mBeam[lIdxBeam].CellValueTbl[lIdxCell]=rand()%128;
        }
        // Test clock mode
        else if (pVideoMode == OBX_VIDEO_TEST_CLOCK)
        {
          // Second angle
          {
            int lSecondAngleDeg = lNow.second()*6;
            double lAzGapDeg = fabs(lCellAzDeg - lSecondAngleDeg);
            lAzGapDeg = lAzGapDeg*(1-(1-lCellRgRto)*mVideoAzGapCorrectionRto);
            double lAzLvl = qBound(0.0, 1 - mVideoAzGapLvlRto * lAzGapDeg, 1.0);
            int    lLvl = 255 * mVideoIntensity * lAzLvl;

            if (lCellRgRto < 0.9)
            {
              mBeam[lIdxBeam].CellValueTbl[lIdxCell]= qMin(255, mBeam[lIdxBeam].CellValueTbl[lIdxCell]+lLvl);
            }
          }

          // Minute angle
          {
            int lMinuteAngleDeg = lNow.minute()*6;
            double lAzGapDeg = fabs(lCellAzDeg - lMinuteAngleDeg);
            lAzGapDeg = lAzGapDeg*(1-(1-lCellRgRto)*mVideoAzGapCorrectionRto);
            double lAzLvl = qBound(0.0, 1 - mVideoAzGapLvlRto * lAzGapDeg, 1.0);
            int    lLvl = 255 * mVideoIntensity * lAzLvl;

            if (lCellRgRto < 0.6)
            {
              mBeam[lIdxBeam].CellValueTbl[lIdxCell]= qMin(255, mBeam[lIdxBeam].CellValueTbl[lIdxCell]+lLvl);
            }
          }

          // Hour angle
          {
            int lHourAngleDeg   = (lNow.hour()%12)*30;
            double lAzGapDeg = fabs(lCellAzDeg - lHourAngleDeg);
            lAzGapDeg = lAzGapDeg*(1-(1-lCellRgRto)*mVideoAzGapCorrectionRto);
            double lAzLvl = qBound(0.0, 1 - mVideoAzGapLvlRto * lAzGapDeg, 1.0);
            int    lLvl = 255 * mVideoIntensity * lAzLvl;

            if (lCellRgRto < 0.3)
            {
              mBeam[lIdxBeam].CellValueTbl[lIdxCell]= qMin(255, mBeam[lIdxBeam].CellValueTbl[lIdxCell]+lLvl);
            }
          }





        }
        // Test Random mode
        else if (pVideoMode == OBX_VIDEO_TEST)
        {
          if (lIdxCell == 0)
          {
            mBeam[lIdxBeam].CellValueTbl[0]=lIdxBeam%255;
          }
          else
          {
            mBeam[lIdxBeam].CellValueTbl[lIdxCell]=mBeam[lIdxBeam].CellValueTbl[0];
          }
        }
      }

      //
      lIdxBeam++;
    }
  }
}
