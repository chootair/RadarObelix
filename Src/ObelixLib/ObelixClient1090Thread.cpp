#include "ObelixClient1090Thread.h"




#define GETBIT(m,b) ((m >> b) & 1U)
#define SETBIT(m,b) (m |= 1UL << b)


ObelixClient1090Thread::ObelixClient1090Thread(QHostAddress pHostAddr, uint pPortNo, QObject *parent):
  QThread(parent),mRun(false), mHostAddr(pHostAddr), mPortNo(pPortNo), mAircaftTablePtr(nullptr), mRwlocker(nullptr)
{
}

ObelixClient1090Thread::~ObelixClient1090Thread()
{
}

void ObelixClient1090Thread::SetTrackTableParameter(QHash<uint16_t, T_ObelixTrack> *pAircaftTablePtr, QReadWriteLock *pRwlocker)
{
  mAircaftTablePtr = pAircaftTablePtr;
  mRwlocker        = pRwlocker;
}

void ObelixClient1090Thread::AskForStop()
{
    mRun = false;
}

void ObelixClient1090Thread::run()
{
  QTcpSocket    lClient;
  char          lReadBuffer[1024];
  QStringList   lReadMsgList;
  T_1090MsgSbs1 lSb1Msg;
  qint64        lReadSz = 0;

  // Valid track table parameters
  if (!mAircaftTablePtr && !mRwlocker)
  {
    qWarning("Invalid track table parameters");
    return;
  }

  // Control flag
  mRun = true;

  // Connection loop
  while(mRun)
  {
    lClient.connectToHost(mHostAddr, static_cast<quint16>(mPortNo));
    if (true == lClient.waitForConnected(5000))
    {
      qDebug("Connected to server [OK]");
      break;
    }
    else
    {
      qDebug("Cannot connect to server: %s", lClient.errorString().toStdString().c_str()) ;
    }
  }

  // Read Loop
  while (mRun)
  {
    // Reset
    lReadMsgList.clear();
    memset(lReadBuffer, 0, 1024);

    // Wait data
    if (lClient.waitForReadyRead(10000) == true)
    {
      // Read message
      lReadSz = lClient.read(lReadBuffer, 1024);
      if (lReadSz == -1)
      {
        qWarning("Read data failed: %s", lClient.errorString().toStdString().c_str());
      }
      else
      {
        lReadMsgList = QString(lReadBuffer).split('\n');

        // Loop on messages
        foreach (QString lMsg, lReadMsgList)
        {
          // Empty message
          if (lMsg.isEmpty() == true)
          {
            continue;
          }

          // Parse message
          if (ParseSbs1Message(lMsg, lSb1Msg))
          {
            //
            mRwlocker->lockForWrite();

            // Aircraft Id
            uint16_t lAfctId = static_cast<uint16_t>(lSb1Msg.HexModeSId);

            // Add
            if (mAircaftTablePtr->contains(lAfctId) == false)
            {
              T_ObelixTrack lNewTrack;

              memset(&lNewTrack, 0, sizeof(T_ObelixTrack));

              // Init values
              lNewTrack.Id         = lAfctId;
              lNewTrack.AircraftId = lSb1Msg.HexModeSId;

              /// \todo Set invalid values

              // Insert
              mAircaftTablePtr->insert(lAfctId, lNewTrack);
            }


            // Update
            if (mAircaftTablePtr->contains(lAfctId) == true)
            {
              // Call sign
              if (lSb1Msg.Callsign.isEmpty() != true)
              {
                sprintf((*mAircaftTablePtr)[lAfctId].CallSing, "%s", lSb1Msg.Callsign.toStdString().c_str());
              }
              // Altitude
              if (GETBIT(lSb1Msg.IsValid, SBS1MSG_IDX_ALT) == 1)
              {
                (*mAircaftTablePtr)[lAfctId].Altitude  = static_cast<quint16>(lSb1Msg.Altitude/OBX_TRK_ALT_LSB);
                (*mAircaftTablePtr)[lAfctId].ModeCCode = static_cast<quint16>(lSb1Msg.Altitude/100);  // Mode C Altitude x100Ft
              }
              // Ground speed
              if (GETBIT(lSb1Msg.IsValid, SBS1MSG_IDX_SPD) == 1)
              {
                (*mAircaftTablePtr)[lAfctId].GroundSpeed = static_cast<quint16>(lSb1Msg.GroundSpeed/OBX_TRK_SPEED_LSB);
              }
              // Track - Course
              if (GETBIT(lSb1Msg.IsValid, SBS1MSG_IDX_TRK) == 1)
              {
                (*mAircaftTablePtr)[lAfctId].Course = static_cast<quint16>(lSb1Msg.Track/OBX_TRK_BEARINGCOURSE_LSB);
              }
              // Latitude
              if (GETBIT(lSb1Msg.IsValid, SBS1MSG_IDX_LAT) == 1)
              {
                (*mAircaftTablePtr)[lAfctId].Latitude = static_cast<qint32>(lSb1Msg.Latitude/OBX_TRK_LATLONG_LSB);
              }
              // Longitude
              if (GETBIT(lSb1Msg.IsValid, SBS1MSG_IDX_LON) == 1)
              {
                (*mAircaftTablePtr)[lAfctId].Longitude = static_cast<qint32>(lSb1Msg.Longitude/OBX_TRK_LATLONG_LSB);
              }

              // Transponder
              (*mAircaftTablePtr)[lAfctId].Mode3ACode = static_cast<quint16>(lSb1Msg.Squawk);        // Mode A Squawk code
              (*mAircaftTablePtr)[lAfctId].ModeSCode  = static_cast<quint16>(lSb1Msg.HexModeSId);
            }

            //
            mRwlocker->unlock();
          }
        }
      }
    }
    else
    {
      qDebug("No data to read");
    }
  }
}


bool ObelixClient1090Thread::ParseSbs1Message(QString &pInMessage, T_1090MsgSbs1 &pOutMessage)
{
  bool lIsOk= false;

  // Split message
  QStringList lParseData = pInMessage.split(',');
  if (lParseData.count() != 22)
  {
    return false;
  }

  //
  pOutMessage.MessageType      = lParseData.at(0).toUInt();
  pOutMessage.TransmissionType = lParseData.at(1).toUInt();
  pOutMessage.SessionId        = lParseData.at(2).toUInt();
  pOutMessage.AircraftId       = lParseData.at(3).toUInt();
  pOutMessage.HexModeSId       = lParseData.at(4).toUInt(&lIsOk,16);
  pOutMessage.FlightId         = lParseData.at(5).toUInt();
  pOutMessage.GeneratedDate    = QDate::fromString(lParseData.at(6));
  pOutMessage.GeneratedTime    = QTime::fromString(lParseData.at(7));
  pOutMessage.LoggedDate       = QDate::fromString(lParseData.at(8));
  pOutMessage.LoggedTime       = QTime::fromString(lParseData.at(9));
  pOutMessage.Callsign         = lParseData.at(10);
  pOutMessage.Altitude         = lParseData.at(SBS1MSG_IDX_ALT).toDouble();
  pOutMessage.GroundSpeed      = lParseData.at(SBS1MSG_IDX_SPD).toDouble();
  pOutMessage.Track            = lParseData.at(SBS1MSG_IDX_TRK).toDouble();
  pOutMessage.Latitude         = lParseData.at(SBS1MSG_IDX_LAT).toDouble();
  pOutMessage.Longitude        = lParseData.at(SBS1MSG_IDX_LON).toDouble();
  pOutMessage.VerticalRate     = lParseData.at(SBS1MSG_IDX_VRS).toDouble();
  pOutMessage.Squawk           = lParseData.at(17).toUInt();
  pOutMessage.Alert            = lParseData.at(18).toUInt();
  pOutMessage.Emergency        = lParseData.at(19).toUInt();
  pOutMessage.SPI              = lParseData.at(20).toUInt();
  pOutMessage.IsOnGround       = lParseData.at(21).toUInt();


  // Reset valid flag
  pOutMessage.IsValid = 0;

  // Set valid flag
  for (int i=0; i<22; i++)
  {
    if (lParseData.at(i).isEmpty() != true)
    {
      SETBIT(pOutMessage.IsValid, i);
    }
  }

  //
  return true;
}





