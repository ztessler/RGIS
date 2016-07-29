#include <cm.h>

static size_t _Iteration = 10000;

static void _UserFunc(void *commonPtr, void *threadData, size_t taskId) {
    size_t count;
    for (count = 0; count < _Iteration; count++);
}

int main(int argv, char *argc[]) {
    int ret, threadNum = 1;
    size_t loopNum = 4, timeLoop, taskNum = 10000, taskId;
    CMthreadTeam_p team = (CMthreadTeam_p) NULL;
    CMthreadJob_p job;

    if ((argv > 1) && (sscanf(argc[1], "%d", &ret) == 1)) threadNum = (size_t) ret > 0 ? ret : 1;
    if ((argv > 2) && (sscanf(argc[2], "%d", &ret) == 1)) taskNum = (size_t) ret;
    if ((argv > 3) && (sscanf(argc[3], "%d", &ret) == 1)) _Iteration = (size_t) ret;
    if ((argv > 4) && (sscanf(argc[4], "%d", &ret) == 1)) loopNum = (size_t) ret;
    printf("%d %d %d %d\n", (int) threadNum, (int) taskNum, (int) _Iteration, (int) loopNum);

    team = CMthreadTeamCreate(threadNum);
    if ((job = CMthreadJobCreate(team, (void *) NULL, taskNum, _UserFunc)) == (CMthreadJob_p) NULL) {
        CMmsgPrint(CMmsgAppError, "Job creation error in %s:%d\n", __FILE__, __LINE__);
        CMthreadTeamDestroy(team);
        return (CMfailed);
    }
    for (taskId = 0; taskId < taskNum; ++taskId)
        CMthreadJobTaskDependent(job, taskId, (taskId | 0x0000000f) < taskNum ? (taskId | 0x0000000f) : taskNum - 1);

    for (timeLoop = 0; timeLoop < loopNum; ++timeLoop) {
        printf("Time %d\n", (int) timeLoop);
        CMthreadJobExecute(team, job);
    }
    CMthreadJobDestroy(job);
    CMthreadTeamDestroy(team);
    return (0);
}
