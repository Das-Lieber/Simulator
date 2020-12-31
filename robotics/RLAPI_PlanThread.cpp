#include "RLAPI_PlanThread.h"
#include <QDebug>

//=======================================================================
//function : RLAPI_PlanThread
//purpose  : *
//=======================================================================
RLAPI_PlanThread::RLAPI_PlanThread(const rl::mdl::Dynamic &aDynamic, rl::sg::pqp::Scene &aScene, const double &minModelSize,
                                   QObject *parent) : QThread(parent),minMdlSize(minModelSize)
{
    mDynamic = new rl::mdl::Dynamic(aDynamic);
    mPlanScene = &aScene;//point to the scene in the main thread
}

//=======================================================================
//function : RLAPI_PlanThread
//purpose  : *
//=======================================================================
RLAPI_PlanThread::~RLAPI_PlanThread()
{
    delete mDynamic;
    mDynamic = nullptr;
    mPlanScene = nullptr;//if delete here, pointer in the main thread will be held on
}

//=======================================================================
//function : GetComputeArguments
//purpose  : *
//=======================================================================
void RLAPI_PlanThread::GetComputeArguments(const rl::math::Vector &startPnt, const rl::math::Vector &endPnt)
{
    mStartPnt = startPnt;
    mEndPnt = endPnt;
}

//=======================================================================
//function : run
//purpose  : *
//=======================================================================
void RLAPI_PlanThread::run()
{
    rl::plan::SimpleModel *mPlanModel = new rl::plan::SimpleModel();
    rl::plan::AdvancedOptimizer *mPathOptimizer = new rl::plan::AdvancedOptimizer;
    rl::plan::GnatNearestNeighbors nearestNeighbors(mPlanModel);
    rl::plan::Prm planner;
    rl::plan::UniformSampler sampler;
    rl::plan::RecursiveVerifier verifier;

    mPlanModel->mdl = mDynamic;
    mPlanModel->model = mPlanScene->getModel(0);
    mPlanModel->scene = mPlanScene;

    verifier.delta = 10.0f * rl::math::constants::deg2rad;
    verifier.model = mPlanModel;

    mPathOptimizer->model = mPlanModel;
    mPathOptimizer->verifier = &verifier;
    mPathOptimizer->ratio = 0.01;
    mPathOptimizer->length = 0.2*minMdlSize;

    sampler.model = mPlanModel;

    planner.start = &mStartPnt;
    planner.goal = &mEndPnt;
    planner.model = mPlanModel;
    planner.verifier = &verifier;
    planner.setNearestNeighbors(&nearestNeighbors);
    planner.sampler = &sampler;

    //verify the start and the goal.if there is an invalid value ,thread will be crashed
    if(!planner.verify())
    {
        emit ComputeFailed(ComputeError::InvalidConfig);
        return;
    }

    PlannerSolved = false;
    QTimer aComputeTimer;
    aComputeTimer.singleShot(8000,this,[=](){
        if(!PlannerSolved)
            emit ComputeTimeOut();
        return;
    });

    QElapsedTimer aElapsedTimer;
    aElapsedTimer.start();

    PlannerSolved = planner.solve();

    if(PlannerSolved)
    {
        emit ComputeSuccess(aElapsedTimer.elapsed());

        rl::plan::VectorList solvedPath = planner.getPath();

        verifier.delta *= 2;
        mPathOptimizer->process(solvedPath);
        qDebug()<<"compute+optimize "<<aElapsedTimer.elapsed();

        rl::math::Vector tmpPos(mPlanModel->getDofPosition());

        rl::plan::VectorList::iterator i = solvedPath.begin();
        rl::plan::VectorList::iterator j = ++solvedPath.begin();

        //interpolation of the path
        //1、add the start
        if (i != solvedPath.end() && j != solvedPath.end())
        {
            emit ReadyToSetJointValue(*i);
            QThread::usleep(50000);
        }

        rl::math::Real delta = 10.0f;
        double pathLength = 0;

        //2、add the interpolations until the goal        
        for (; i != solvedPath.end() && j != solvedPath.end(); ++i, ++j)
        {
            pathLength += mPlanModel->distance(*i ,*j);
            rl::math::Real stepLength = std::ceil(mPlanModel->distance(*i, *j) / delta);
            for (std::size_t k = 1; k < stepLength + 1; ++k)
            {
                if(!isInterruptionRequested())
                {
                    mPlanModel->interpolate(*i, *j, k/stepLength, tmpPos);
                    emit ReadyToSetJointValue(tmpPos);
                    QThread::usleep(50000);
                }
            }
        }
        emit ComputeOver(pathLength);
    }
    else
    {
        emit ComputeFailed(ComputeError::ArgumentError);
        return;
    }
    delete mPlanModel;
    delete mPathOptimizer;
}
