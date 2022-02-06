#include "ObelixPlotWidget.h"
#include "ObelixToolbox.h"



ObelixPlotWidget::ObelixPlotWidget(QWidget *parent) : QOpenGLWidget(parent)
{

  //
  mFifoObelixVideoLoad = 0;
  mFifoObelixVideo.mFifoIndexPtr    = new uint;
  *(mFifoObelixVideo.mFifoIndexPtr) = 0;
  mFifoObelixVideo.mMessageSize     = sizeof(T_ObelixVideoMessage);
  mFifoObelixVideo.mFifoSize        = 1024;
  mFifoObelixVideo.mFifoPtr         = (char*)calloc(mFifoObelixVideo.mFifoSize, sizeof(T_ObelixVideoMessage));
  mFifoObelixVideo.mFifoLocker      = new QReadWriteLock();
  //
  mFifoVideoPtr = (T_ObelixVideoMessage*)(mFifoObelixVideo.mFifoPtr);


  //
  mFifoObelixTrackLoad = 0;
  mFifoObelixTrack.mFifoIndexPtr    = new uint;
  *(mFifoObelixTrack.mFifoIndexPtr) = 0;
  mFifoObelixTrack.mMessageSize     = sizeof(T_ObelixTrackReportMessage);
  mFifoObelixTrack.mFifoSize        = 1024;
  mFifoObelixTrack.mFifoPtr         = (char*)calloc(mFifoObelixVideo.mFifoSize, sizeof(T_ObelixTrackReportMessage));
  mFifoObelixTrack.mFifoLocker      = new QReadWriteLock();
  //
  mFifoTrackPtr = (T_ObelixTrackReportMessage*)(mFifoObelixTrack.mFifoPtr);

  //
  mFifoObelixMapLoad = 0;
  mFifoObelixMap.mFifoIndexPtr    = new uint;
  *(mFifoObelixMap.mFifoIndexPtr) = 0;
  mFifoObelixMap.mMessageSize     = sizeof(T_ObelixMapMessage);
  mFifoObelixMap.mFifoSize        = 1024;
  mFifoObelixMap.mFifoPtr         = (char*)calloc(mFifoObelixMap.mFifoSize, sizeof(T_ObelixMapMessage));
  mFifoObelixMap.mFifoLocker      = new QReadWriteLock();
  //
  mFifoMapPtr = (T_ObelixMapMessage*)(mFifoObelixMap.mFifoPtr);


  mTrackTable.clear();


  mLastTrackPlotUpdate = 0;


  
  mScopeVideoImg = new QImage(800, 800, QImage::Format_ARGB32_Premultiplied);
  mScopeTrackImg = new QImage(800, 800, QImage::Format_ARGB32_Premultiplied);
  mToolsImg      = new QImage(800, 800, QImage::Format_ARGB32_Premultiplied);
  mObelixMapImg  = new QImage(800, 800, QImage::Format_ARGB32_Premultiplied);
  mShapeMapImg   = new QImage(800, 800, QImage::Format_ARGB32_Premultiplied);
  mHeadingImg    = new QImage(800, 800, QImage::Format_ARGB32_Premultiplied);
  mWidgetImg     = new QImage(800, 800, QImage::Format_ARGB32_Premultiplied);

  
  mPersistImg = new PresistImage(800,800);
  mPersistImg->SetPersistence(0);
  mPersistRatio = -1;
  mPersistMultiplyColor.setRgb(0,0,0);
  mPersistMode = PersistComposition;


  mShapeMapManager = new MapManager();
  
  mLbxWidget = new QLabel("SCOPE RADAR", this);
  mLbxWidget->lower();
  
  mSideMargin = 20;
  mHrzRatio = 0.99;
  mRefSize=0;
  mPlotRad=0;
  mXCtr=0;
  mYCtr=0;
  
  mElapsedAverageTimeMs = 0;
  mElapsedMonitorCnt = 0;
  mElapsedTime = 0;

  mLastAzimuthDeg = 0;
  mLastHeadingDeg = 0;



  mDisplayPxKtsRatio = 1;
  mVideoIntensity = 0.8;
  
  
  mRangeNm = 100;
  mRangeRingSpaceNm = 25;


  mDisplayAntenna = true;
  mDisplayRangeLimit = true;
  mDisplayRangeRings = true;
  mDisplayCompas = true;
  mDisplayVideo = true;
  mDisplayTracks = true;
  mDisplayAircraft = true;
  mDisplayMapPoints = true;
  mDisplayMapPolygons = true;
  mDisplayShapeMap    = true;
  //
  mNorthUp = true;
  //
  mDisplayPlateformHistory = false;
  mDisplayTracksHistory = false;
  //
  mSelectedTrackIdx = -1;
  mHistoryDisplayMaxTimeSec = 30;

  mColorAntenna     = Qt::green;
  mColorRangeLimit  = Qt::green;
  mColorRangeRings  = Qt::green;
  mColorCompas      = Qt::green;
  mColorVideo       = Qt::green;
  mColorTracks      = Qt::white;
  mColorAircraft    = Qt::yellow;
  mColorHeading     = Qt::red;
  mColorMapPoints   = Qt::darkYellow;
  mColorMapPolygons = Qt::darkRed;
  mColorMapShape    = Qt::darkMagenta;
  mColorPlateformHistory = Qt::darkYellow;
  mColorTracksHistory    = Qt::darkGray;
  mColorSelectedTrack    = Qt::magenta;


  mNeedToPaintInfo = true;
  mNeedToPaintShapeMap = true;


}

ObelixPlotWidget::~ObelixPlotWidget()
{
  delete mFifoObelixVideo.mFifoLocker;
  delete mFifoObelixVideo.mFifoIndexPtr;
  free(mFifoObelixVideo.mFifoPtr);
  //
  delete mFifoObelixTrack.mFifoLocker;
  delete mFifoObelixTrack.mFifoIndexPtr;
  free(mFifoObelixTrack.mFifoPtr);
  //
  delete mFifoObelixMap.mFifoLocker;
  delete mFifoObelixMap.mFifoIndexPtr;
  free(mFifoObelixMap.mFifoPtr);

  //
  delete mShapeMapManager;

}

void ObelixPlotWidget::RefreshScope()
{
  mElapsedTimer.start();
  
  //
  ReadTrackPlots();

  //
  ReadMapPlots();

  //
  PaintControl();
  
  //
  mElapsedTime = mElapsedTime + mElapsedTimer.elapsed();
  mElapsedMonitorCnt++;
  //
  //qDebug("%i %i %i %i", mElapsedAverageTimeMs, mElapsedMonitorCnt, mElapsedTime, mElapsedTime / mElapsedMonitorCnt);
  //
  if (mElapsedMonitorCnt >= 30)
  {
    mElapsedAverageTimeMs = mElapsedTime / mElapsedMonitorCnt;
    mElapsedMonitorCnt = 0;
    mElapsedTime = 0;
  }
}

void ObelixPlotWidget::ClearScope()
{
  mWidgetImg->fill(0xFF000000);
  mScopeVideoImg->fill(0xFF000000);
  mScopeTrackImg->fill(0x00000000);
  mHeadingImg->fill(0x00000000);
  mToolsImg->fill(0x00000000);
  mObelixMapImg->fill(0x00000000);
  mShapeMapImg->fill(0x00000000);
  mPersistImg->fill(0xFF000000);

  //
  mNeedToPaintInfo=true;
  mNeedToPaintShapeMap = true;

  //
  RefreshScope();
}

bool ObelixPlotWidget::AddShapeMapFile(QString pShapeFilename)
{
  mShapeMapManager->LoadShapeFile(pShapeFilename);

  //
  return true;
}

void ObelixPlotWidget::SetPresistenceRatio(double pRatio)
{
  mPersistImg->SetPersistence(pRatio);
  mPersistRatio = pRatio;
  mPersistMultiplyColor.setRgb(240*pRatio,240*pRatio,240*pRatio);
}

void ObelixPlotWidget::resizeEvent(QResizeEvent *event)
{
  Q_UNUSED(event);
  SetMyGeometry();
}



void ObelixPlotWidget::SetMyGeometry()
{
  mRefSize = qMin(this->width(),(int)(mHrzRatio*this->height()))/2 - mSideMargin;
  mXCtr    = this->width()/2;
  mYCtr    = this->height()/2;
  mPlotRad       = 0.96 * mRefSize;
  
  
  // Image plot display --------------------------
  mLbxWidget->setGeometry(0, 0, this->width(), this->height());
  //
  delete mWidgetImg;
  mWidgetImg = new QImage(this->width(), this->height(), QImage::Format_ARGB32_Premultiplied);
  //
  delete mPersistImg;
  mPersistImg = new PresistImage(2*mPlotRad, 2*mPlotRad);
  mPersistImg->SetPersistence(0.98);
  //
  delete mScopeVideoImg;
  mScopeVideoImg = new QImage(2*mPlotRad, 2*mPlotRad, QImage::Format_ARGB32_Premultiplied);
  //
  delete mHeadingImg;
  mHeadingImg = new QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
  //
  delete mToolsImg;
  mToolsImg = new QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
  //
  delete mObelixMapImg;
  mObelixMapImg = new QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
  //
  delete mShapeMapImg;
  mShapeMapImg = new QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
  //
  delete mScopeTrackImg;
  mScopeTrackImg = new QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);

  // Reset images
  mWidgetImg->fill(0xFF000000);
  mScopeVideoImg->fill(0xFF000000);
  mScopeTrackImg->fill(0x00000000);
  mHeadingImg->fill(0x00000000);
  mToolsImg->fill(0x00000000);
  mObelixMapImg->fill(0x00000000);
  mShapeMapImg->fill(0x00000000);

  //
  mNeedToPaintInfo=true;
}

void ObelixPlotWidget::PaintVideoCells(QPainter *pPainter)
{
  double lAzimuthOffsetDeg = 0;

  pPainter->setPen(Qt::NoPen);
  QBrush lBrush(Qt::black);

  // Lock FIFO
  mFifoObelixVideo.mFifoLocker->lockForWrite();
  
  // FIFO Load
  mFifoObelixVideoLoad = *(mFifoObelixVideo.mFifoIndexPtr)/(float)mFifoObelixVideo.mFifoSize*100.0;

  // Reset antenna position
  if (*(mFifoObelixVideo.mFifoIndexPtr) >= 1)
  {
    mLastAzimuthDeg = 0;
  }

  // Reset heading
  if (*(mFifoObelixVideo.mFifoIndexPtr) > 0)
  {
    double lCurrentHeading = mLastHeadingDeg;

    //
    mLastHeadingDeg = mFifoVideoPtr[0].HeadingDeg;
    if (mLastHeadingDeg > 360)
    {
      mLastHeadingDeg = mLastHeadingDeg - 360;
    }
    if (mLastHeadingDeg < 0)
    {
      mLastHeadingDeg = mLastHeadingDeg + 360;
    }

    if (lCurrentHeading != mLastHeadingDeg)
    {
      mNeedToPaintInfo = true;
    }
  }


  // Heading Up
  if (mNorthUp == false)
  {
    lAzimuthOffsetDeg = -mLastHeadingDeg;
  }
  
  // Loop on message
  for (uint i=0; i<(*(mFifoObelixVideo.mFifoIndexPtr)); i++)
  {
    double lCosStart = cos((lAzimuthOffsetDeg + mFifoVideoPtr[i].StartAzDeg) * OTB::DEG_TO_RAD);
    double lSinStart = sin((lAzimuthOffsetDeg + mFifoVideoPtr[i].StartAzDeg) * OTB::DEG_TO_RAD);
    double lCosEnd   = cos((lAzimuthOffsetDeg + mFifoVideoPtr[i].EndAzDeg  ) * OTB::DEG_TO_RAD);
    double lSinEnd   = sin((lAzimuthOffsetDeg + mFifoVideoPtr[i].EndAzDeg  ) * OTB::DEG_TO_RAD);

    mLastAzimuthDeg = qMax(mLastAzimuthDeg, mFifoVideoPtr[i].EndAzDeg);
    
    //
    mCellPt[0].setX(mPlotRad + mPlotRad/mRangeNm*(mFifoVideoPtr[i].StartRgNm + 0*mFifoVideoPtr[i].CellWidthNm)*lSinStart);
    mCellPt[0].setY(mPlotRad - mPlotRad/mRangeNm*(mFifoVideoPtr[i].StartRgNm + 0*mFifoVideoPtr[i].CellWidthNm)*lCosStart);
    mCellPt[1].setX(mPlotRad + mPlotRad/mRangeNm*(mFifoVideoPtr[i].StartRgNm + 0*mFifoVideoPtr[i].CellWidthNm)*lSinEnd);
    mCellPt[1].setY(mPlotRad - mPlotRad/mRangeNm*(mFifoVideoPtr[i].StartRgNm + 0*mFifoVideoPtr[i].CellWidthNm)*lCosEnd);
    
    for(uint16_t lIdxCell=1; lIdxCell<= mFifoVideoPtr[i].CellCount; lIdxCell++)
    {
      mCellPt[3].setX(mPlotRad + mPlotRad/mRangeNm*(mFifoVideoPtr[i].StartRgNm + lIdxCell*mFifoVideoPtr[i].CellWidthNm)*lSinStart);
      mCellPt[3].setY(mPlotRad - mPlotRad/mRangeNm*(mFifoVideoPtr[i].StartRgNm + lIdxCell*mFifoVideoPtr[i].CellWidthNm)*lCosStart);
      mCellPt[2].setX(mPlotRad + mPlotRad/mRangeNm*(mFifoVideoPtr[i].StartRgNm + lIdxCell*mFifoVideoPtr[i].CellWidthNm)*lSinEnd);
      mCellPt[2].setY(mPlotRad - mPlotRad/mRangeNm*(mFifoVideoPtr[i].StartRgNm + lIdxCell*mFifoVideoPtr[i].CellWidthNm)*lCosEnd);
      
      // Fill cell according radar mode
      if ((mFifoVideoPtr[i].VideoMode == OBX_VIDEO_SEARCH    )||
          (mFifoVideoPtr[i].VideoMode == OBX_VIDEO_TEST_CLOCK))
      {
        double lFactor = mVideoIntensity * mFifoVideoPtr[i].CellValueTbl[lIdxCell-1]/255.0;


        lBrush.setColor(QColor(lFactor*mColorVideo.red(),
                               lFactor*mColorVideo.green(),
                               lFactor*mColorVideo.blue()));
      }
      else if (mFifoVideoPtr[i].VideoMode == OBX_VIDEO_WEATHER)
      {
        QColor lWeatherColor = Qt::black;

        // Weather color scale
        if (mFifoVideoPtr[i].CellValueTbl[lIdxCell-1] < 10)
        {
          lWeatherColor = Qt::black;
        }
        else if (mFifoVideoPtr[i].CellValueTbl[lIdxCell-1] < 20)
        {
          lWeatherColor = Qt::cyan;
        }
        else if (mFifoVideoPtr[i].CellValueTbl[lIdxCell-1] < 30)
        {
          lWeatherColor = Qt::blue;
        }
        else if (mFifoVideoPtr[i].CellValueTbl[lIdxCell-1] < 40)
        {
          lWeatherColor = Qt::green;
        }
        else if (mFifoVideoPtr[i].CellValueTbl[lIdxCell-1] < 50)
        {
          lWeatherColor = Qt::darkGreen;
        }
        else if (mFifoVideoPtr[i].CellValueTbl[lIdxCell-1] < 60)
        {
          lWeatherColor = Qt::yellow;
        }
        else if (mFifoVideoPtr[i].CellValueTbl[lIdxCell-1] < 70)
        {
          lWeatherColor = Qt::darkYellow;
        }
        else if (mFifoVideoPtr[i].CellValueTbl[lIdxCell-1] < 80)
        {
          lWeatherColor = QColor(255,165,0); // Orange
        }
        else if (mFifoVideoPtr[i].CellValueTbl[lIdxCell-1] < 90)
        {
          lWeatherColor = Qt::red;
        }
        else if (mFifoVideoPtr[i].CellValueTbl[lIdxCell-1] < 100)
        {
          lWeatherColor = Qt::darkRed;
        }
        else
        {
          lWeatherColor = Qt::magenta;
        }

        // Intensity modulation
        lBrush.setColor(QColor(mVideoIntensity*lWeatherColor.red(),
                               mVideoIntensity*lWeatherColor.green(),
                               mVideoIntensity*lWeatherColor.blue()));
      }
      else if (mFifoVideoPtr[i].VideoMode == OBX_VIDEO_TEST)
      {
        QColor lTestColor = Qt::black;

        int lCode = mFifoVideoPtr[i].CellValueTbl[lIdxCell-1] % 3;

        if (lCode == 0)
        {
          lTestColor = Qt::red;
        }
        else if (lCode == 1)
        {
          lTestColor = Qt::green;
        }
        else
        {
          lTestColor = Qt::blue;
        }

        // Intensity modulation
        lBrush.setColor(QColor(mVideoIntensity*lTestColor.red(),
                               mVideoIntensity*lTestColor.green(),
                               mVideoIntensity*lTestColor.blue()));
      }

      else
      {
        /// \todo No Brush ?
      }

      
      //
      pPainter->setBrush(lBrush);
      pPainter->drawPolygon(mCellPt,4);
      
      //
      mCellPt[0] = mCellPt[3];
      mCellPt[1] = mCellPt[2];
      
    }
  }
  
  // Reset index
  *(mFifoObelixVideo.mFifoIndexPtr) = 0;
  
  // Unlock FIFO
  mFifoObelixVideo.mFifoLocker->unlock();
}

void ObelixPlotWidget::PaintTrackPlots(QPainter *pPainter)
{
  double lAzimuthOffsetRad = 0;


  pPainter->setBrush(Qt::NoBrush);


  QPoint lTrackPt;
  QPoint lVectorPt;

  // Heading Up
  if (mNorthUp == false)
  {
    lAzimuthOffsetRad = -mLastHeadingDeg*OTB::DEG_TO_RAD;
  }

  // Loop on plot track
  foreach (T_PlotTrack lPlotTrack, mTrackTable)
  {

    // Skip track outside display range
    if (lPlotTrack.Track.Distance*OBX_TRK_DISTANCE_LSB > mRangeNm)
    {
      continue;
    }

    // Old Track -> Force to change its status

    // Very Old Track -> Remove

    /// \todo Add LSB
    /// \todo Add Speed display factor
    ///
    ///
    ///
    ///

    // Track Position
    double lTrackRangePx    = mPlotRad/mRangeNm*lPlotTrack.Track.Distance*OBX_TRK_DISTANCE_LSB;
    double lTrackBreaingRad = static_cast<double>(lPlotTrack.Track.Bearing)*OBX_TRK_BEARINGCOURSE_LSB*OTB::DEG_TO_RAD;
    //
    lTrackPt.setX(width()/2  + static_cast<int>(lTrackRangePx * sin(lAzimuthOffsetRad + lTrackBreaingRad)));
    lTrackPt.setY(height()/2 - static_cast<int>(lTrackRangePx * cos(lAzimuthOffsetRad + lTrackBreaingRad)));

    // Speed Vector
    double lTrackSpeedKts = static_cast<double>(lPlotTrack.Track.GroundSpeed)*OBX_TRK_SPEED_LSB;
    double lTrackSpeedPx   = mDisplayPxKtsRatio*lTrackSpeedKts;
    //
    double lTrackCourseDeg = static_cast<double>(lPlotTrack.Track.Course)*OBX_TRK_BEARINGCOURSE_LSB;
    double lTrackCourseRad = lTrackCourseDeg*OTB::DEG_TO_RAD;
    //
    lVectorPt.setX(lTrackPt.x() + static_cast<int>(lTrackSpeedPx*sin(lAzimuthOffsetRad + lTrackCourseRad)));
    lVectorPt.setY(lTrackPt.y() - static_cast<int>(lTrackSpeedPx*cos(lAzimuthOffsetRad + lTrackCourseRad)));

    // Age
    double lUpdateAgeRto = 100 * (QDateTime::currentMSecsSinceEpoch() - lPlotTrack.LastUpdate)/10000;
    int lAgeCount = lUpdateAgeRto /25;
    QString lAgeString = "";
    for (int i=0; i<lAgeCount; i++)
    {
      lAgeString.append(".");
    }


    // Track color
    if ((mSelectedTrackIdx >=0) && (lPlotTrack.Track.Id == mSelectedTrackIdx))
    {
      pPainter->setPen(mColorSelectedTrack);
    }
    else
    {
      pPainter->setPen(mColorTracks);
    }

    // Paint
    pPainter->drawRect(lTrackPt.x()-3, lTrackPt.y()-3, 6,6);
    pPainter->drawLine(lTrackPt, lVectorPt);
    pPainter->drawText(lTrackPt.x()+10, lTrackPt.y()+7,QString("[%1]%2").arg(lPlotTrack.Track.Id).arg(lAgeString));
    pPainter->drawText(lTrackPt.x()+10, lTrackPt.y()+22,QString("%1°/%2kts").arg(static_cast<int>(lTrackCourseDeg),3,10,QChar('0'))
                       .arg(static_cast<int>(lTrackSpeedKts),3,10,QChar('0')));
    pPainter->drawText(lTrackPt.x()+10, lTrackPt.y()+37,QString("%1").arg(lPlotTrack.Track.CallSing));
  }
}

void ObelixPlotWidget::PaintHeadingFeatures(QPainter *pPainter)
{
  // Translate painter
  pPainter->translate(mXCtr,mYCtr);

  // Rotate for Aircraft & Heading
  if (mNorthUp == true)
  {
    pPainter->rotate(mLastHeadingDeg);
  }

  // Heading
  if (mDisplayHeading == true)
  {
    pPainter->setPen(mColorHeading);
    pPainter->drawLine(0, 0, 0, -0.8*mPlotRad);
  }

  // Aircraft
  if (mDisplayAircraft == true)
  {
    int lAftB = 6;
    int lAftWidth = 15;
    int lAftHeight = 10;

    pPainter->setPen(QPen(mColorAircraft,2));
    pPainter->drawLine(0,-10,0,15);
    pPainter->drawLine(-lAftWidth,0,lAftWidth,0);
    pPainter->drawLine(-5,10,5,10);
  }
}

void ObelixPlotWidget::PaintTools(QPainter *pPainter)
{
  // Translate painter
  pPainter->translate(mXCtr,mYCtr);

  // Range limit
  if (mDisplayRangeLimit == true)
  {
    pPainter->setPen(mColorRangeLimit);
    pPainter->drawEllipse(-mPlotRad, -mPlotRad, 2*mPlotRad, 2*mPlotRad);
  }

  // Range rings
  if (mDisplayRangeRings == true)
  {
    pPainter->setPen(mColorRangeRings);

    uint lRangeRingSpace = mRangeRingSpaceNm;

    // Ring space auto ?
    if (lRangeRingSpace <= 0)
    {
      lRangeRingSpace = static_cast<uint>(10*floor(mRangeNm/30.0));
      lRangeRingSpace = qMax(lRangeRingSpace, (uint)5);
    }

    // Loop on range rings
    for (uint lRangeRingDst=lRangeRingSpace; lRangeRingDst<mRangeNm; lRangeRingDst += lRangeRingSpace)
    {
      int lRangeScale = mPlotRad/mRangeNm*lRangeRingDst;
      pPainter->drawEllipse(-lRangeScale, -lRangeScale, 2*lRangeScale, 2*lRangeScale);
    }
  }

  // Compas
  if (mDisplayCompas == true)
  {
    double lAzimuthOffsetDeg = 0;

    //
    pPainter->setPen(mColorCompas);

    // Heading Up
    if (mNorthUp == false)
    {
      lAzimuthOffsetDeg = -mLastHeadingDeg;
    }

    //
    int lMarkWidth  = 0;
    int lMarkLength = 0;

    //
    for (int iDeg = 0; iDeg < 360; iDeg+=5)
    {
      // Major
      if (iDeg%90 == 0)
      {
        lMarkWidth = 5;
        lMarkLength = 10;
      }
      // Medium
      else if (iDeg%45 == 0)
      {
        lMarkWidth = 3;
        lMarkLength=5;
      }
      // Small
      else if (iDeg%10 == 0)
      {
        lMarkWidth = 1;
        lMarkLength=3;
      }
      else
      {
        continue;
      }

      double lMarkAngleRad = (lAzimuthOffsetDeg+iDeg)*OTB::DEG_TO_RAD;

      // Mark
      pPainter->setPen(QPen(mColorCompas,lMarkWidth));
      pPainter->drawLine((mPlotRad            ) * sin(lMarkAngleRad), -(mPlotRad            ) * cos(lMarkAngleRad),
                         (mPlotRad+lMarkLength) * sin(lMarkAngleRad), -(mPlotRad+lMarkLength) * cos(lMarkAngleRad));


      // Text
      if (iDeg%10 == 0)
      {
        QSize        sz_l;
        QFontMetrics ft_metrics_l (this->font());
        QString lText = QString("%1").arg(iDeg,3,10,QChar('0'));

        if (iDeg == 0)
        {
          lText = lText + QString("[N]");
        }

        //
        sz_l = ft_metrics_l.size(Qt::TextSingleLine, lText);
        int lTextRad = 30;

        pPainter->drawText( (mPlotRad+lTextRad) * sin(lMarkAngleRad)  - 0.5*sz_l.width(),
                            -(mPlotRad+lTextRad) * cos(lMarkAngleRad) + 0.5*sz_l.height(),lText);// ,Qt::AlignVCenter,sz_l.width(),sz_l.height(),lText);
      }


    }
  }
}

void ObelixPlotWidget::PaintObelixMap(QPainter *pPainter)
{
  // Translate painter
  //pPainter->translate(mXCtr,mYCtr);

  double lAzimuthOffsetRad = 0;


  // Heading Up
  if (mNorthUp == false)
  {
    lAzimuthOffsetRad = -mLastHeadingDeg*OTB::DEG_TO_RAD;
  }




  // Platform position
  double lPlatformX = OTB::LonToX(mLastMapPlatformLongitude);
  double lPlatformY = OTB::LatToY(mLastMapPlatformLatitude);

  // Loop on plot map object
  foreach (T_PlotMap lPlotMap, mMapTable)
  {
    QPolygon polygon;

    // Default
    pPainter->setPen(Qt::red);
    pPainter->setPen(QPen(QBrush(mColorMapPoints),5));

    foreach (T_ObelixMapPoint lPlotPoint, lPlotMap.Points)
    {
      double lPixelRatio = mPlotRad/(mRangeNm * OTB::NM_TO_M);
      double lPtLat  = static_cast<double>(lPlotPoint.Latitude)  * OBX_TRK_LATLONG_LSB;
      double lPtLong = static_cast<double>(lPlotPoint.Longitude) * OBX_TRK_LATLONG_LSB;

      double lPtX =   (OTB::LonToX(lPtLong) - lPlatformX) * lPixelRatio;
      double lPtY =  -(OTB::LatToY(lPtLat)  - lPlatformY) * lPixelRatio;


      double lPtAz = 0;
      double lPtDist = 0;


      OTB::ComputeAzimuthDistance(mLastMapPlatformLatitude, mLastMapPlatformLongitude, lPtLat, lPtLong, lPtAz, lPtDist);


      lPtDist = lPtDist * lPixelRatio;
      lPtAz   = lPtAz * OTB::DEG_TO_RAD;

      // Point Position
      lPtX = width()/2  + static_cast<int>(lPtDist * sin(lAzimuthOffsetRad + lPtAz));
      lPtY = height()/2 - static_cast<int>(lPtDist * cos(lAzimuthOffsetRad + lPtAz));


      if (lPlotMap.Type == OBX_MAP_POLY)
      {
        polygon.append(QPoint(lPtX, lPtY));
      }
      // Single point object
      else if ((lPlotMap.Type == OBX_MAP_SINGLE) && (mDisplayMapPoints == true))
      {
        QSize        sz_l;
        QFontMetrics ft_metrics_l (this->font());
        QString lText = QString(lPlotPoint.Label);


        pPainter->drawPoint(lPtX, lPtY);

        //
        sz_l = ft_metrics_l.size(Qt::TextSingleLine, lText);
        int lTextRad = 30;

        pPainter->drawText( lPtX - 0.5*sz_l.width(),
                            lPtY + 0.99*sz_l.height(),lText);// ,Qt::AlignVCenter,sz_l.width(),sz_l.height(),lText);
      }
    }


    if ((lPlotMap.Type == OBX_MAP_POLY) && (mDisplayMapPolygons == true))
    {
      pPainter->setPen(mColorMapPolygons);
      QColor lFillColor = mColorMapPolygons;
      lFillColor.setAlpha(100);
      pPainter->setBrush(lFillColor);
      pPainter->drawPolygon(polygon);
    }
  }
  
  // History
  
  // Platform history
  if (true == mDisplayPlateformHistory)
  {
    pPainter->setPen(mColorPlateformHistory);
    pPainter->drawLines(BuildHistroryLine(mPlatformHistory, lAzimuthOffsetRad));
  }
  
  // Track history
  if (true == mDisplayTracksHistory)
  {
    foreach(T_PlotTrack lTrack, mTrackTable)
    {
      if ((mSelectedTrackIdx >=0) && (lTrack.Track.Id == mSelectedTrackIdx))
      {
        pPainter->setPen(mColorSelectedTrack.darker());
      }
      else
      {
        pPainter->setPen(mColorTracksHistory);
      }

      //
      pPainter->drawLines(BuildHistroryLine(lTrack.History, lAzimuthOffsetRad));
    }
  }
}


void ObelixPlotWidget::PaintShapeMap(QPainter *pPainter)
{
  double lAzimuthOffsetRad = 0;

  double lPixelRatio = mPlotRad/(mRangeNm * OTB::NM_TO_M);

  // Heading Up
  //if (mNorthUp == false)
  //{
  //  lAzimuthOffsetRad = -mLastHeadingDeg*OTB::DEG_TO_RAD;
  //}

  //
  mShapeMapCenterLat = mLastMapPlatformLatitude;
  mShapeMapCenterLon = mLastMapPlatformLongitude;



  // Get objects in area
  QList<T_ShapeMapObject> lShapeMapObjectList = mShapeMapManager->GetObjectInRange(mLastMapPlatformLongitude,mLastMapPlatformLatitude,200);

  // Loop on shape map manager objects
  foreach (T_ShapeMapObject lShapeObject, lShapeMapObjectList)
  {
    QPolygon polygon;

    // Default
    pPainter->setPen(Qt::red);
    pPainter->setPen(QPen(QBrush(mColorMapPoints),5));

    foreach (T_ShapeMapPoint lPlotPoint, lShapeObject.Points)
    {
      double lPtAz = 0;
      double lPtDist = 0;

      OTB::ComputeAzimuthDistance(mLastMapPlatformLatitude, mLastMapPlatformLongitude, lPlotPoint.Latitude, lPlotPoint.Longitude, lPtAz, lPtDist);

      if (lPtDist > (400*OTB::NM_TO_M))
      {
        continue;
      }

      lPtDist = lPtDist * lPixelRatio;
      lPtAz   = lPtAz * OTB::DEG_TO_RAD;

      // Point Position
      int lPtX = width()/2  + static_cast<int>(lPtDist * sin(lAzimuthOffsetRad + lPtAz));
      int lPtY = height()/2 - static_cast<int>(lPtDist * cos(lAzimuthOffsetRad + lPtAz));

      polygon.append(QPoint(lPtX, lPtY));
    }


    pPainter->setPen(mColorMapShape);
    pPainter->drawPolyline(polygon);
  }
}


QVector<QPointF> ObelixPlotWidget::BuildHistroryLine(T_History pHistory, double pAzimuthOffsetRad)
{
  QVector<QPointF> lHistoryLine;
  //
  double lPixelRatio = mPlotRad/(mRangeNm * OTB::NM_TO_M);
  double lPtAz       = 0;
  double lPtDist     = 0;
  //
  qint64 lNow = QDateTime::currentMSecsSinceEpoch();

  foreach(T_HistoryPoint lHistPoint, pHistory)
  {
    // Too old
    if (lNow > (lHistPoint.Date+1000*mHistoryDisplayMaxTimeSec))
    {
      continue;
    }

    OTB::ComputeAzimuthDistance(mLastMapPlatformLatitude, mLastMapPlatformLongitude, lHistPoint.Latitude,  lHistPoint.Longitude, lPtAz, lPtDist);


    lPtDist = lPtDist * lPixelRatio;
    lPtAz   = lPtAz * OTB::DEG_TO_RAD;

    // Point Position
    int lPtX = width()/2  + static_cast<int>(lPtDist * sin(pAzimuthOffsetRad + lPtAz));
    int lPtY = height()/2 - static_cast<int>(lPtDist * cos(pAzimuthOffsetRad + lPtAz));

    lHistoryLine.append(QPointF(lPtX, lPtY));
  }

  return lHistoryLine;
}















void ObelixPlotWidget::PaintControl()
{
  double lAzimuthOffset = 0;
  QPainter lScopeVideoPainter(mScopeVideoImg);
  QPainter lWidgetPainter(mWidgetImg);

  // Reset
  mWidgetImg->fill(0xFF000000);

  if (mPersistRatio >= 0)
  {
    mScopeVideoImg->fill(0xFF000000);
  }

  // Cell table
  if (mDisplayVideo == true)
  {
    // Painter
    PaintVideoCells(&lScopeVideoPainter);

    // Enable persistance
    if (mPersistRatio > 0)
    {
      if (PersistComposition == mPersistMode)
      {
        QPainter lPersistPainter(mPersistImg);

        // Light the previous image
        if (mPersistRatio<1)
        {
          lPersistPainter.setCompositionMode( QPainter::CompositionMode_Multiply);
          lPersistPainter.fillRect(0,0,mPersistImg->width(), mPersistImg->height(),QBrush(mPersistMultiplyColor));
        }

        // Add the new
        lPersistPainter.setCompositionMode( QPainter::CompositionMode_Plus);
        lPersistPainter.drawImage(0,0, *mScopeVideoImg);
      }
      else if (PersistPixel == mPersistMode)
      {
        mPersistImg->AppendImage(*mScopeVideoImg);
      }
      else
      {
        qDebug("Oops");
      }

      // Paint into the widget image
      lWidgetPainter.drawImage(mXCtr-mPersistImg->width()/2, mYCtr-mPersistImg->height()/2, *mPersistImg);
    }
    else
    {
      lWidgetPainter.drawImage(mXCtr-mScopeVideoImg->width()/2, mYCtr-mScopeVideoImg->height()/2, *mScopeVideoImg);
    }
  }

  // Paint tracks
  if (mDisplayTracks == true)
  {
    QPainter lScopeTrackPainter(mScopeTrackImg);
    qint64 lCurrentUpdate = QDateTime::currentMSecsSinceEpoch();

    // Update track image
    /// \todo Set via paramerter
    if ((lCurrentUpdate - mLastTrackPlotUpdate) > 200)
    {
      mScopeTrackImg->fill(0x00000000);
      PaintTrackPlots(&lScopeTrackPainter);

      mLastTrackPlotUpdate = lCurrentUpdate;
    }

    lWidgetPainter.drawImage(0, 0, *mScopeTrackImg);
  }

  // Info
  if (mNeedToPaintInfo == true)
  {
    QPainter lToolsPainter(mToolsImg);
    mToolsImg->fill(0x00000000);
    PaintTools(&lToolsPainter);
    mNeedToPaintInfo = false;
  }
  lWidgetPainter.drawImage(0, 0, *mToolsImg);


  // Obelix Map
  {
    QPainter lMapPainter(mObelixMapImg);
    mObelixMapImg->fill(0x00000000);
    PaintObelixMap(&lMapPainter);
    lWidgetPainter.drawImage(0, 0, *mObelixMapImg);
  }

  // Shape Map
  if (mDisplayShapeMap)
  {
    double lPtAz = 0;
    double lPtDist = 0;

    OTB::ComputeAzimuthDistance(mLastMapPlatformLatitude, mLastMapPlatformLongitude, mShapeMapCenterLat, mShapeMapCenterLon, lPtAz, lPtDist);

    if (lPtDist > 50*OTB::NM_TO_M)
    {
      mNeedToPaintShapeMap = true;
    }


    if (mNeedToPaintShapeMap == true)
    {
    QPainter lShapeMapPainter(mShapeMapImg);
    mShapeMapImg->fill(0x00000000);
    PaintShapeMap(&lShapeMapPainter);
    mNeedToPaintShapeMap = false;
    }


    OTB::ComputeAzimuthDistance(mLastMapPlatformLatitude, mLastMapPlatformLongitude, mShapeMapCenterLat, mShapeMapCenterLon, lPtAz, lPtDist);

    double lPixelRatio = mPlotRad/(mRangeNm * OTB::NM_TO_M);
    lPtDist = lPtDist * lPixelRatio;
    lPtAz   = lPtAz * OTB::DEG_TO_RAD;

    int xOffset = -width()/2 + static_cast<int>(lPtDist * sin(lPtAz));
    int yOffset = -height()/2 - static_cast<int>(lPtDist * cos(lPtAz));
    double aOffset = 0;

    // Heading Up
    if (mNorthUp == false)
    {
      aOffset = -mLastHeadingDeg;
    }

    lWidgetPainter.save();
    lWidgetPainter.translate(width()/2,height()/2);
    lWidgetPainter.rotate(aOffset);

    lWidgetPainter.drawImage(xOffset, yOffset, *mShapeMapImg);
    lWidgetPainter.restore();
  }

  // Paint heading
  if ((mDisplayAircraft == true) || (mDisplayHeading == true))
  {
    QPainter lHeadingPainter(mHeadingImg);
    mHeadingImg->fill(0x00000000);
    PaintHeadingFeatures(&lHeadingPainter);
    lWidgetPainter.drawImage(0, 0, *mHeadingImg);
  }

  // Antenna
  if (mDisplayAntenna == true)
  {
    if (mNorthUp == false)
    {
      lAzimuthOffset = -mLastHeadingDeg;
    }
    else
    {
      lAzimuthOffset = 0;
    }

    lWidgetPainter.setPen(mColorAntenna);
    lWidgetPainter.drawLine(mXCtr, mYCtr, mXCtr+mPlotRad*sin((lAzimuthOffset+mLastAzimuthDeg)*OTB::DEG_TO_RAD), mYCtr-mPlotRad*cos((lAzimuthOffset+mLastAzimuthDeg)*OTB::DEG_TO_RAD));
  }

  //
  mLbxWidget->setPixmap(QPixmap::fromImage(*mWidgetImg));
}


int ObelixPlotWidget::ReadMapPlots()
{
  // Lock FIFO
  mFifoObelixMap.mFifoLocker->lockForWrite();

  // FIFO load
  mFifoObelixMapLoad = *(mFifoObelixMap.mFifoIndexPtr)/(float)mFifoObelixMap.mFifoSize*100.0;


  //qDebug("ReadMapPlots %i", *(mFifoObelixMap.mFifoIndexPtr));

  // Loop on message
  for (uint i=0; i<(*(mFifoObelixMap.mFifoIndexPtr)); i++)
  {
    // Known element ?
    if (mMapTable.contains(mFifoMapPtr[i].ElementId) == true)
    {
      // Append
      if (mFifoMapPtr[i].OperationType == OBX_MAP_APPEND)
      {
        //mMapTable[mFifoMapPtr[i].ElementId].Count += mFifoMapPtr[i].PointCount;

        //
        for (int idxPt=0; idxPt<mFifoMapPtr[i].PointCount; idxPt++)
        {
          mMapTable[mFifoMapPtr[i].ElementId].Points.append(mFifoMapPtr[i].PointTbl[idxPt]);
        }
      }
      // Delete
      else if (mFifoMapPtr[i].OperationType == OBX_MAP_DELETE)
      {
        mMapTable.remove(mFifoMapPtr[i].ElementId);
      }
    }
    else
    {
      // Add new element
      if ((mFifoMapPtr[i].OperationType == OBX_MAP_APPEND) ||
          (mFifoMapPtr[i].OperationType == OBX_MAP_UPDATE))
      {
        T_PlotMap lNewMapElement;

        //
        lNewMapElement.Type  = mFifoMapPtr[i].ElementType;
        //lNewMapElement.Count = mFifoMapPtr[i].PointCount;

        //
        for (int idxPt=0; idxPt<mFifoMapPtr[i].PointCount; idxPt++)
        {
          lNewMapElement.Points.append(mFifoMapPtr[i].PointTbl[idxPt]);
        }

        // Add to map
        mMapTable[mFifoMapPtr[i].ElementId] = lNewMapElement;
      }
    }

    // Last platfrom position
    mLastMapPlatformLatitude  = static_cast<double>(mFifoMapPtr[i].PlatformLatitude) * OBX_TRK_LATLONG_LSB;
    mLastMapPlatformLongitude = static_cast<double>(mFifoMapPtr[i].PlatformLongitude)* OBX_TRK_LATLONG_LSB;
    mLastMapPlatformHeading   = static_cast<double>(mFifoMapPtr[i].PlatformHeading)  * OBX_TRK_BEARINGCOURSE_LSB;

    // Append to history
    T_HistoryPoint lHistPt;
    lHistPt.Date      = QDateTime::currentMSecsSinceEpoch();
    lHistPt.Latitude  = mLastMapPlatformLatitude;
    lHistPt.Longitude = mLastMapPlatformLongitude;
    if (mPlatformHistory.count() > 1)
    {
      if ((mPlatformHistory.last().Latitude == mLastMapPlatformLatitude) &&
          (mPlatformHistory.last().Longitude == mLastMapPlatformLongitude) )
      {
        mPlatformHistory.takeLast();
      }
    }

    mPlatformHistory.append(lHistPt);
    if (mPlatformHistory.count() > 200)
    {
      mPlatformHistory.takeFirst();
    }
  }

  // Reset index
  *(mFifoObelixMap.mFifoIndexPtr) = 0;

  // Unlock FIFO
  mFifoObelixMap.mFifoLocker->unlock();

  //
  return 0;
}


int ObelixPlotWidget::ReadTrackPlots()
{
  // Lock FIFO
  mFifoObelixTrack.mFifoLocker->lockForWrite();

  // FIFO load
  mFifoObelixTrackLoad = *(mFifoObelixTrack.mFifoIndexPtr)/(float)mFifoObelixTrack.mFifoSize*100.0;

  // Loop on message
  for (uint i=0; i<(*(mFifoObelixTrack.mFifoIndexPtr)); i++)
  {
    // Loop on tracks
    for (uint j=0; j<mFifoTrackPtr[i].TrackCount; j++)
    {
      // Update
      if (mTrackTable.contains(mFifoTrackPtr[i].TrackTbl[j].Id) == true)
      {
        mTrackTable[mFifoTrackPtr[i].TrackTbl[j].Id].Track      = mFifoTrackPtr[i].TrackTbl[j];
        mTrackTable[mFifoTrackPtr[i].TrackTbl[j].Id].LastUpdate = QDateTime::currentMSecsSinceEpoch();
      }
      // Add
      else
      {
        T_PlotTrack lNewTrack;
        //
        lNewTrack.Track      = mFifoTrackPtr[i].TrackTbl[j];
        lNewTrack.LastUpdate = QDateTime::currentMSecsSinceEpoch();
        //
        mTrackTable.insert(lNewTrack.Track.Id, lNewTrack);
      }

      // History
      T_HistoryPoint lHistPt;
      lHistPt.Date      = QDateTime::currentMSecsSinceEpoch();
      lHistPt.Latitude  = static_cast<double>(mTrackTable[mFifoTrackPtr[i].TrackTbl[j].Id].Track.Latitude)*OBX_TRK_LATLONG_LSB;
      lHistPt.Longitude = static_cast<double>(mTrackTable[mFifoTrackPtr[i].TrackTbl[j].Id].Track.Longitude)*OBX_TRK_LATLONG_LSB;;



      if (mTrackTable[mFifoTrackPtr[i].TrackTbl[j].Id].History.count() > 1)
      {
        if ((mTrackTable[mFifoTrackPtr[i].TrackTbl[j].Id].History.last().Latitude == lHistPt.Latitude) &&
            (mTrackTable[mFifoTrackPtr[i].TrackTbl[j].Id].History.last().Longitude == lHistPt.Longitude) )
        {
          mTrackTable[mFifoTrackPtr[i].TrackTbl[j].Id].History.takeLast();
        }
      }


      mTrackTable[mFifoTrackPtr[i].TrackTbl[j].Id].History.append(lHistPt);


      if (mTrackTable[mFifoTrackPtr[i].TrackTbl[j].Id].History.count() > 200)
      {
        mTrackTable[mFifoTrackPtr[i].TrackTbl[j].Id].History.takeFirst();
      }
    }
  }

  // Reset index
  *(mFifoObelixTrack.mFifoIndexPtr) = 0;

  // Unlock FIFO
  mFifoObelixTrack.mFifoLocker->unlock();

  // Remove old tracks


  //
  return 0;
}
