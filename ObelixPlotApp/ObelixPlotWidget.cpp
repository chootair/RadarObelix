#include "ObelixPlotWidget.h"

#define _USE_MATH_DEFINES
#include <math.h>

static const double DEG_TO_RAD = M_PI/180;

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


  mTrackTable.clear();


  mLastTrackPlotUpdate = 0;


  
  mScopeVideoImg    = new QImage(800, 800, QImage::Format_ARGB32_Premultiplied);
  mWidgetImg     = new QImage(800, 800, QImage::Format_ARGB32_Premultiplied);
  
  mPersistImg = new GreenPresistImage(800,800);
  mPersistImg->SetPersistence(0);
  mIsPersistEnabled = false;
  
  mLbxWidget = new QLabel("SCOPE RADAR", this);
  mLbxWidget->lower();
  
  mSideMargin = 3;
  mHrzRatio = 0.99;
  mRefSize=0;
  mPlotRad=0;
  mXCtr=0;
  mYCtr=0;
  
  mElapsedAverageTimeMs = 0;
  mElapsedMonitorCnt = 0;
  mElapsedTime = 0;

  mLastAzimuthDeg = 0;
  
  
  mRangeNm = 100;
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
}

void ObelixPlotWidget::RefreshScope()
{
  mElapsedTimer.start();
  
  //
  ReadTrackPlots();

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
}

void ObelixPlotWidget::SetPresistenceRatio(double pRatio)
{
  mPersistImg->SetPersistence(pRatio);
  if (pRatio <= 0)
  {
    mIsPersistEnabled = false;
  }
  else
  {
    mIsPersistEnabled = true;
  }
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
  mPersistImg = new GreenPresistImage(2*mPlotRad, 2*mPlotRad);
  mPersistImg->SetPersistence(0.98);
  //
  delete mScopeVideoImg;
  mScopeVideoImg = new QImage(2*mPlotRad, 2*mPlotRad, QImage::Format_ARGB32_Premultiplied);
  
  // Reset images
  mWidgetImg->fill(0xFF000000);
  mScopeVideoImg->fill(0xFF000000);
}

void ObelixPlotWidget::PaintVideoCells(QPainter *pPainter)
{


  pPainter->setPen(Qt::NoPen);
  QBrush lBrush(Qt::black);
  
  // Lock FIFO
  mFifoObelixVideo.mFifoLocker->lockForWrite();
  
  //qDebug("%i message to plot", *mFifoIndexPtr);
  mFifoObelixVideoLoad = *(mFifoObelixVideo.mFifoIndexPtr)/(float)mFifoObelixVideo.mFifoSize*100.0;
  
  //qDebug("%i %i %i",mFifoObelixVideoLoad, *mFifoIndexPtr, mFifoSize);
  

  if (*(mFifoObelixVideo.mFifoIndexPtr) >= 1)
  {
    mLastAzimuthDeg = 0;
  }
  
  // Loop on message
  for (uint i=0; i<(*(mFifoObelixVideo.mFifoIndexPtr)); i++)
  {
    double lCosStart = cos(mFifoVideoPtr[i].StartAzDeg*DEG_TO_RAD);
    double lSinStart = sin(mFifoVideoPtr[i].StartAzDeg*DEG_TO_RAD);
    double lCosEnd   = cos(mFifoVideoPtr[i].EndAzDeg*DEG_TO_RAD);
    double lSinEnd   = sin(mFifoVideoPtr[i].EndAzDeg*DEG_TO_RAD);
    
    
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
      
      //
      lBrush.setColor(QColor(0,mFifoVideoPtr[i].CellValueTbl[lIdxCell-1],0));
      
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
  pPainter->setPen(Qt::red);
  pPainter->setBrush(Qt::NoBrush);


  QDateTime lNow = QDateTime::currentDateTime();
  QPoint lTrackPt;
  QPoint lVectorPt;

  // Loop on plot track
  foreach (T_PlotTrack lPlotTrack, mTrackTable)
  {

    // Old Track -> Force to change its status

    // Very Old Track -> Remove

    /// \todo Add LSB
    /// \todo Add Speed display factor


    // Track Position
    lTrackPt.setX(width()/2 + mPlotRad/mRangeNm*lPlotTrack.Track.DistanceNm * sin(lPlotTrack.Track.BearingDeg*DEG_TO_RAD));
    lTrackPt.setY(height()/2 - mPlotRad/mRangeNm*lPlotTrack.Track.DistanceNm * cos(lPlotTrack.Track.BearingDeg*DEG_TO_RAD));

    // Speed Vector
    lVectorPt.setX(lTrackPt.x()+lPlotTrack.Track.GroundSpeedKts*sin(lPlotTrack.Track.CourseDeg*DEG_TO_RAD));
    lVectorPt.setY(lTrackPt.y()-lPlotTrack.Track.GroundSpeedKts*cos(lPlotTrack.Track.CourseDeg*DEG_TO_RAD));

    // Paint
    pPainter->drawRect(lTrackPt.x()-4, lTrackPt.y()-4, 8,8);
    pPainter->drawLine(lTrackPt, lVectorPt);
    pPainter->drawText(lTrackPt.x()+6, lTrackPt.y()+6,QString(lPlotTrack.Track.CallSing));
  }
}


void ObelixPlotWidget::PaintControl()
{
  // Reset
  mWidgetImg->fill(0xFF000000);
  if (true == mIsPersistEnabled)
  {
    mScopeVideoImg->fill(0xFF000000);
  }
  
  QPainter lScopePainter(mScopeVideoImg);
  QPainter lWidgetPainter(mWidgetImg);
  
  
  // Cell table
  PaintVideoCells(&lScopePainter);


  
  //
  
  if (true == mIsPersistEnabled)
  {
    // Persistance on full scope
    mPersistImg->AppendImage(*mScopeVideoImg);
    lWidgetPainter.drawImage(mXCtr-mPersistImg->width()/2, mYCtr-mPersistImg->height()/2, *mPersistImg);
  }
  else
  {
    lWidgetPainter.drawImage(mXCtr-mScopeVideoImg->width()/2, mYCtr-mScopeVideoImg->height()/2, *mScopeVideoImg);
  }
  
  
  lWidgetPainter.setPen(Qt::green);
  
  //lWidgetPainter.drawText(10,10,QString("%1").arg(mFpsAverageTimeMs));
  lWidgetPainter.drawEllipse(mXCtr-mPlotRad, mYCtr-mPlotRad, 2*mPlotRad, 2*mPlotRad);
  
  
  lWidgetPainter.drawLine(mXCtr, mYCtr, mXCtr+mPlotRad*sin(mLastAzimuthDeg*DEG_TO_RAD), mYCtr-mPlotRad*cos(mLastAzimuthDeg*DEG_TO_RAD));
  

  PaintTrackPlots(&lWidgetPainter);
  
  //
  mLbxWidget->setPixmap(QPixmap::fromImage(*mWidgetImg));
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
        mTrackTable[mFifoTrackPtr[i].TrackTbl[j].Id].LastUpdate = QDateTime::currentSecsSinceEpoch();
      }
      // Add
      else
      {
        T_PlotTrack lNewTrack;
        //
        lNewTrack.Track      = mFifoTrackPtr[i].TrackTbl[j];
        lNewTrack.LastUpdate = QDateTime::currentSecsSinceEpoch();
        //
        mTrackTable.insert(lNewTrack.Track.Id, lNewTrack);
      }
    }
  }

  // Reset index
  *(mFifoObelixTrack.mFifoIndexPtr) = 0;

  // Unlock FIFO
  mFifoObelixTrack.mFifoLocker->unlock();

  //
  return 0;
}
