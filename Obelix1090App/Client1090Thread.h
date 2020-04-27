#ifndef SBS1CLIENTTHREAD_H
#define SBS1CLIENTTHREAD_H

#include <QObject>
#include <QThread>
#include <QReadWriteLock>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDateTime>


typedef struct _T_1090MsgSbs1
{
  uint    MessageType;      // (MSG,STA, ID, AIR, SEL or CLK)
  uint    TransmissionType; // MSGsub types 1 to 8. Not used by other message types.
  uint    SessionId;        // Database Session record number
  uint    AircraftId;       // DatabaseAircraft record number
  uint    HexModeSId;       // AircraftMode S hexadecimal code
  uint    FlightId;         // DatabaseFlight record number
  QDate   GeneratedDate;
  QTime   GeneratedTime;
  QDate   LoggedDate;
  QTime   LoggedTime;
  QString Callsign;     // An eight digit flight ID - can be flight number or registration (or evennothing).
  double  Altitude;     // ModeC altitude. Height relative to 1013.2mb (Flight Level). Not heightAMSL..  :
  double  GroundSpeed;  // Speedover ground (not indicated airspeed)
  double  Track;        // Track of aircraft (not heading). Derived from the velocity E/W and velocityN/S
  double  Latitude;     // Northand East positive. South and West negative.
  double  Longitude;    // Northand East positive. South and West negative.
  double  VerticalRate; // 64 ft resolution
  uint    Squawk;       // AssignedMode A squawk code.
  uint    Alert;        // Flag to indicate squawk has changed.
  uint    Emergency;    // Flag to indicate emergency code has been set
  uint    SPI;          // Flag to indicate transponder Ident has been activated.
  uint    IsOnGround;   // Flag to indicate ground squat switch is active
  quint32 IsValid;      // Valid falgs
}T_1090MsgSbs1;



static const uint SBS1MSG_IDX_ALT = 11;
static const uint SBS1MSG_IDX_SPD = 12;
static const uint SBS1MSG_IDX_TRK = 13;
static const uint SBS1MSG_IDX_LAT = 14;
static const uint SBS1MSG_IDX_LON = 15;
static const uint SBS1MSG_IDX_VRS = 16;



class Client1090Thread : public QThread
{
    Q_OBJECT
public:
  explicit Client1090Thread(QHostAddress pHostAddr, uint pPortNo, QObject *parent = 0);
  virtual ~Client1090Thread();

  void SetTrackTableParameter(QHash<uint, T_1090MsgSbs1>* pAircaftTablePtr,
                              QReadWriteLock* pRwlocker);

  void AskForStop();



private:
   void run();
  bool ParseSbs1Message(QString &pInMessage, T_1090MsgSbs1 &pOutMessage);

private:
    bool mRun;
    //
  QHostAddress mHostAddr;
  uint         mPortNo;
  //
  QHash<uint, T_1090MsgSbs1>* mAircaftTablePtr;
  QReadWriteLock* mRwlocker;
};

#endif // SBS1CLIENTTHREAD_H
