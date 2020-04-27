#include "Client1090Thread.h"




#define GETBIT(m,b) ((m >> b) & 1U)
#define SETBIT(m,b) (m |= 1UL << b)


Client1090Thread::Client1090Thread(QHostAddress pHostAddr, uint pPortNo, QObject *parent):
  QThread(parent),mRun(false), mHostAddr(pHostAddr), mPortNo(pPortNo), mAircaftTablePtr(nullptr), mRwlocker(nullptr)
{
}

Client1090Thread::~Client1090Thread()
{
}

void Client1090Thread::SetTrackTableParameter(QHash<uint, T_1090MsgSbs1> *pAircaftTablePtr, QReadWriteLock *pRwlocker)
{
  mAircaftTablePtr = pAircaftTablePtr;
  mRwlocker        = pRwlocker;
}

void Client1090Thread::AskForStop()
{
    mRun = false;
}

void Client1090Thread::run()
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

            // Update
            if (mAircaftTablePtr->contains(lSb1Msg.HexModeSId) == true)
            {
              // Call sign
              if (lSb1Msg.Callsign.isEmpty() != true)
              {
                (*mAircaftTablePtr)[lSb1Msg.HexModeSId].Callsign = lSb1Msg.Callsign;
              }
              // Altitude
              if (GETBIT(lSb1Msg.IsValid, SBS1MSG_IDX_ALT) == 1)
              {
                (*mAircaftTablePtr)[lSb1Msg.HexModeSId].Altitude = lSb1Msg.Altitude;
                SETBIT((*mAircaftTablePtr)[lSb1Msg.HexModeSId].IsValid, SBS1MSG_IDX_ALT);
              }
              // Ground speed
              if (GETBIT(lSb1Msg.IsValid, SBS1MSG_IDX_SPD) == 1)
              {
                (*mAircaftTablePtr)[lSb1Msg.HexModeSId].GroundSpeed = lSb1Msg.GroundSpeed;
                SETBIT((*mAircaftTablePtr)[lSb1Msg.HexModeSId].IsValid, SBS1MSG_IDX_SPD);
              }
              // Track
              if (GETBIT(lSb1Msg.IsValid, SBS1MSG_IDX_TRK) == 1)
              {
                (*mAircaftTablePtr)[lSb1Msg.HexModeSId].Track = lSb1Msg.Track;
                SETBIT((*mAircaftTablePtr)[lSb1Msg.HexModeSId].IsValid, SBS1MSG_IDX_TRK);
              }
              // Latitude
              if (GETBIT(lSb1Msg.IsValid, SBS1MSG_IDX_LAT) == 1)
              {
                (*mAircaftTablePtr)[lSb1Msg.HexModeSId].Latitude = lSb1Msg.Latitude;
                SETBIT((*mAircaftTablePtr)[lSb1Msg.HexModeSId].IsValid, SBS1MSG_IDX_LAT);
              }
              // Longitude
              if (GETBIT(lSb1Msg.IsValid, SBS1MSG_IDX_LON) == 1)
              {
                (*mAircaftTablePtr)[lSb1Msg.HexModeSId].Longitude = lSb1Msg.Longitude;
                SETBIT((*mAircaftTablePtr)[lSb1Msg.HexModeSId].IsValid, SBS1MSG_IDX_LON);
              }
              // Verical rate
              if (GETBIT(lSb1Msg.IsValid, SBS1MSG_IDX_VRS) == 1)
              {
                (*mAircaftTablePtr)[lSb1Msg.HexModeSId].VerticalRate = lSb1Msg.VerticalRate;
                SETBIT((*mAircaftTablePtr)[lSb1Msg.HexModeSId].IsValid, SBS1MSG_IDX_VRS);
              }
            }
            // Add
            else
            {
              mAircaftTablePtr->insert(lSb1Msg.HexModeSId, lSb1Msg);
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


bool Client1090Thread::ParseSbs1Message(QString &pInMessage, T_1090MsgSbs1 &pOutMessage)
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




