#include <cm.h>

static size_t _Iteration = 10000;

static void _UserFunc(size_t threadId, size_t taskId, void *commonPtr) {
    size_t count;
    for (count = 0; count < _Iteration; count++);
}

int main(int argv, char *argc[]) {
    int ret, threadNum = 1;
    size_t loopNum = 4, timeLoop, taskNum = 10000, taskId;
    size_t _taskId, _taskNum;
    CMthreadTeam_t team;
    CMthreadJob_p job;

    if ((argv > 1) && (sscanf(argc[1], "%d", &ret) == 1)) threadNum = (size_t) ret > 0 ? ret : 1;
    if ((argv > 2) && (sscanf(argc[2], "%d", &ret) == 1)) taskNum = (size_t) ret;
    if ((argv > 3) && (sscanf(argc[3], "%d", &ret) == 1)) _Iteration = (size_t) ret;
    if ((argv > 4) && (sscanf(argc[4], "%d", &ret) == 1)) loopNum = (size_t) ret;
    printf("%d %d %d %d\n", (int) threadNum, (int) taskNum, (int) _Iteration, (int) loopNum);

    if (CMthreadTeamInitialize(&team, threadNum) == (CMthreadTeam_p) NULL) {
        CMmsgPrint (CMmsgUsrError,"Team initialization error %s, %d",__FILE__,__LINE__);
        return (CMfailed);
    }
    if ((job = CMthreadJobCreate(taskNum, _UserFunc, (void *) NULL)) == (CMthreadJob_p) NULL) {
        CMmsgPrint(CMmsgAppError, "Job creation error in %s:%d", __FILE__, __LINE__);
        CMthreadTeamDestroy(&team);
        return (CMfailed);
    }
    _taskNum = taskNum - 1;
    for (taskId = 0; taskId < taskNum; ++taskId) {
        _taskId = (taskId | 0x0000000f);
        CMthreadJobTaskDependent(job, taskId, _taskId < taskNum ? &_taskId : &_taskNum, 1);
    }

    for (timeLoop = 0; timeLoop < loopNum; ++timeLoop) {
        printf("Time %d\n", (int) timeLoop);
        CMthreadJobExecute(&team, job);
    }
    CMthreadJobDestroy(job);
    CMthreadTeamDestroy(&team);
    CMmsgPrint (CMmsgInfo,"Total Time: %.1f, Execute Time: %.1f, Average Thread Time %.1f, Master Time %.1f",
                (float) team.TotTime    / 1000.0,
                (float) team.ExecTime   / 1000.0,
                (float) team.ThreadTime / (team.ThreadNum > 0 ? (float) team.ThreadNum : 1.0) / 1000.0,
                (float) team.Time       / 1000.0);
    return (0);
}
