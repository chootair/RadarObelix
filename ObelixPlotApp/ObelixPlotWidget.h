#ifndef OBELIXPLOTWIDGET_H
#define OBELIXPLOTWIDGET_H

#include <QOpenGLWidget>
#include <QPainter>
#include <QImage>
#include <QLabel>
#include <QElapsedTimer>
#include <QReadWriteLock>
#include <QDateTime>

#include "Obelix.h"
#include "GreenPresistImage.h"


typedef struct _T_FifoHandler
{
  uint*           mFifoIndexPtr;
  uint            mFifoSize;
  uint            mMessageSize;
  char*           mFifoPtr;
  QReadWriteLock* mFifoLocker;
}T_FifoHandler;


typedef struct _T_PlotTrack
{
  T_ObelixTrack Track;
  qint64        LastUpdate;
}T_PlotTrack;




class ObelixPlotWidget : public QOpenGLWidget
{
  Q_OBJECT
public:
  explicit ObelixPlotWidget(QWidget *parent = nullptr);
  ~ObelixPlotWidget();
  void RefreshScope();
  void ClearScope();

  void SetPresistenceRatio(double pRatio);


  inline double RangeNm() const {return mRangeNm;}
   inline void SetRangeNm(double pRangeNm) {mRangeNm = pRangeNm;}




  inline qint64 ElapsedAverageTimeMs() const {return mElapsedAverageTimeMs;}
  //
  inline uint   FifoObelixVideoLoad() const {return mFifoObelixVideoLoad;}
  inline T_FifoHandler   FifoObelixVideo() {return mFifoObelixVideo;}
  //
  inline uint   FifoObelixTrackLoad() const {return mFifoObelixTrackLoad;}
  inline T_FifoHandler   FifoObelixTrack() {return mFifoObelixTrack;}

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  void PaintControl();
  void SetMyGeometry();
  void PaintVideoCells(QPainter* pPainter);
  void PaintTrackPlots(QPainter* pPainter);
  int ReadTrackPlots();

private:

  QImage* mScopeVideoImg;
  QImage* mWidgetImg;
  GreenPresistImage* mPersistImg;

  bool mIsPersistEnabled;


  double mRangeNm;

  //
  QLabel* mLbxWidget;
  //
  int mSideMargin;
  double graph_ratio_m;
  double spacing_graph_ratio_m;
  double mHrzRatio;
  //
  int mRefSize;
  int mMarkRad;
  int mPlotRad;
  int mXCtr;
  int mYCtr;
  //
      QPoint mCellPt[4];
      //
      double mLastAzimuthDeg;

  //

      QElapsedTimer mElapsedTimer;
      qint64 mElapsedTime;
      qint64 mElapsedMonitorCnt;
      qint64 mElapsedAverageTimeMs;
  //
  T_FifoHandler mFifoObelixVideo;
  T_ObelixVideoMessage* mFifoVideoPtr;
  uint mFifoObelixVideoLoad;

  //
  T_FifoHandler mFifoObelixTrack;
  T_ObelixTrackReportMessage* mFifoTrackPtr;
  uint mFifoObelixTrackLoad;

  //
  QHash<uint16_t, T_PlotTrack> mTrackTable;
  qint64                       mLastTrackPlotUpdate;
};

#endif // OBELIXPLOTWIDGET_H
