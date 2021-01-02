#ifndef RLAPI_PLANTHREAD_H
#define RLAPI_PLANTHREAD_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <QWaitCondition>
#include <QMutex>

#include <rl/plan/SimpleModel.h>
#include <rl/plan/Prm.h>
#include <rl/plan/GnatNearestNeighbors.h>
#include <rl/plan/UniformSampler.h>
#include <rl/plan/RecursiveVerifier.h>
#include <rl/plan/AdvancedOptimizer.h>
#include <rl/sg/pqp/Scene.h>
#include <rl/mdl/Dynamic.h>
#include <rl/math/Constants.h>

enum ComputeError {
    ArgumentError,
    InvalidConfig
};

class RLAPI_PlanThread : public QThread
{
    Q_OBJECT
public:
    //! constructor
    RLAPI_PlanThread(const rl::mdl::Dynamic &aDynamic, rl::sg::pqp::Scene &aScene,
                  const double &minModelSize, QObject *parent = nullptr);

    //! deconstructor
    ~RLAPI_PlanThread();

    //! get the start position and the end position, the position should be described in a vector
    //! which contains the position each link is at
    void GetComputeArguments(const rl::math::Vector &startPnt, const rl::math::Vector &endPnt);

    //! pause the thread
    void pause();

    //! resume the thread if it has been paused
    void resume();

    static bool PlannerSolved;

protected:
    //! override function, solve and optimize the path
    void run() override;

private:
    rl::mdl::Dynamic *mDynamic;
    rl::sg::pqp::Scene *mPlanScene;
    rl::math::Vector mStartPnt;
    rl::math::Vector mEndPnt;
    const double minMdlSize;
    bool doCollisionWait = false;
    QWaitCondition waitCondition;
    QMutex aMutex;

signals:
    //! if planner solve the path, send this signal
    void ComputeSuccess(int time);

    //! send after interpolation, tell the GUI to move the CMM
    //! to thePos position
    void ReadyToSetJointValue(const rl::math::Vector &thePos);

    //! send when all solved, thread finish
    void ComputeOver(double pathLength);

    //! send when planner has computed over 8 seconds
    void ComputeTimeOut();

    //! send if planner doesn't solve the path, often happens
    //! to eet planner
    void ComputeFailed(const ComputeError &error);
};

#endif // RLAPI_PLANTHREAD_H
