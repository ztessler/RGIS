/******************************************************************************

GHAAS Command Line Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2019, UNH - ASRC/CUNY

cmThreads.c

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <signal.h>
#include <cm.h>

size_t CMthreadProcessorNum () {
	char *procEnv;
	int procNum;

	if (((procEnv = getenv ("GHAASprocessorNum")) != (char *) NULL) && (sscanf (procEnv,"%d",&procNum)))
		return (procNum);
	return (0);
}

CMthreadJob_p CMthreadJobCreate (size_t taskNum, CMthreadUserExecFunc execFunc, void *commonData) {
	size_t taskId;
	CMthreadJob_p job;

	if ((job = (CMthreadJob_p) malloc (sizeof (CMthreadJob_t))) == (CMthreadJob_p) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d",__FILE__,__LINE__);
		return ((CMthreadJob_p) NULL);
	}
	if ((job->Tasks = (CMthreadTask_p) calloc (taskNum,sizeof (CMthreadTask_t))) == (CMthreadTask_p) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d",__FILE__,__LINE__);
		free (job);
		return ((CMthreadJob_p) NULL);
	}
	if ((job->SortedTasks = (CMthreadTask_p *) calloc (taskNum,sizeof (CMthreadTask_p))) == (CMthreadTask_p *) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d",__FILE__,__LINE__);
		free (job->Tasks);
		free (job);
		return ((CMthreadJob_p) NULL);
	}
	if ((job->Groups      = (CMthreadTaskGroup_p) calloc (1,sizeof (CMthreadTaskGroup_t))) == (CMthreadTaskGroup_p)  NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d",__FILE__,__LINE__);
		free (job->SortedTasks);
		free (job->Tasks);
		free (job);
		return ((CMthreadJob_p) NULL);
	}
    job->Groups [0].Start = 0;
    job->Groups [0].End   = taskNum;

	job->GroupNum  = 1;
	job->Group     = 0;
	job->Completed = 0;
	job->Sorted    = true;
	job->TaskNum   = taskNum;
	for (taskId = 0;taskId < job->TaskNum; ++taskId) {
		job->SortedTasks [taskId]        = job->Tasks + taskId;
		job->Tasks [taskId].Id           = taskId;
		job->Tasks [taskId].Dependents   = (CMthreadTask_p *) NULL;
		job->Tasks [taskId].NDependents  = 0;
		job->Tasks [taskId].Travel       = 0;
		job->Tasks [taskId].isTravelSet  = 0;

	}
	job->UserFunc = execFunc;
	job->CommonData = (void *) commonData;
	return (job);
}

CMreturn CMthreadJobTaskDependent (CMthreadJob_p job, size_t taskId, size_t *dependents, int dlinknum) {
    int dlink;
	if (taskId    > job->TaskNum) {
		CMmsgPrint (CMmsgAppError,"Invalid task in %s%d",__FILE__,__LINE__);
		return (CMfailed);
	}
    for (dlink = 0; dlink < dlinknum; dlink++)
        if (dependents[dlink] > job->TaskNum) {
            CMmsgPrint (CMmsgAppError,"Invalid dependence in %s%d",__FILE__,__LINE__);
            return (CMfailed);
        }
	if (taskId == *dependents) return (CMsucceeded);
	job->Tasks [taskId].Dependents = (CMthreadTask_p *) malloc (dlinknum * sizeof(CMthreadTask_p));
    if (job->Tasks [taskId].Dependents == (CMthreadTask_p *) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d",__FILE__,__LINE__);
		return (CMfailed);
    }
    for (dlink = 0; dlink < dlinknum; dlink++)
        job->Tasks [taskId].Dependents[dlink] = job->Tasks + dependents[dlink]; // array of pointers to tasks
    job->Tasks [taskId].NDependents = dlinknum;
    job->Sorted = false;
	return (CMsucceeded);
}

static int _CMthreadJobTaskCompare (const void *lPtr,const void *rPtr) {
	int ret;
	CMthreadTask_p lTaskInit = *((CMthreadTask_p *) lPtr);
	CMthreadTask_p rTaskInit = *((CMthreadTask_p *) rPtr);
	CMthreadTask_p lTask = lTaskInit;
	CMthreadTask_p rTask = rTaskInit;

	if ((ret = (int) rTask->Travel - (int) lTask->Travel) != 0) return (ret);

    return 0;
}

size_t _travel_dist(CMthreadTask_t *task) { // RECURSIVE
    int depi;
    size_t travel, maxtravel = 0;
    if (task->isTravelSet) // been set already, quit early
        return task->Travel;
    if (task->Dependents != (CMthreadTask_p *) NULL) {
        for (depi = 0; depi < task->NDependents; depi++) {
            travel = _travel_dist(task->Dependents[depi]) + 1;
            if (travel > maxtravel) {
                maxtravel = travel;
            }
        }
    }
    task->Travel = maxtravel;
    task->isTravelSet = 1;
    return maxtravel;
}

CMreturn _CMthreadJobTaskSort (CMthreadJob_p job) {
	size_t taskId, start;
	size_t travel;
	CMthreadTask_p *dependent;

	for (taskId = 0;taskId < job->TaskNum; ++taskId) {
        // walks downstream along path computing travel (dist from node to mouth)
        // fills in travel dist for each node on path. start from each node to ensure
        // visiting all nodes. skip if cell has already been reached
        if (job->Tasks[taskId].isTravelSet == 0) // unset
            job->Tasks[taskId].Travel = _travel_dist(job->Tasks + taskId);
	}

    qsort (job->SortedTasks,job->TaskNum,sizeof (CMthreadTask_p),_CMthreadJobTaskCompare);
	job->GroupNum = job->SortedTasks [0]->Travel + 1;
	if ((job->Groups = (CMthreadTaskGroup_p) realloc (job->Groups, job->GroupNum * sizeof (CMthreadTaskGroup_t))) == (CMthreadTaskGroup_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s:%d",__FILE__,__LINE__);
		return (CMfailed);
	}
	travel = 0;
	start = 0;
	for (taskId = 0;taskId < job->TaskNum; ++taskId) {
		if (travel != job->GroupNum - job->SortedTasks [taskId]->Travel - 1) {
			job->Groups [travel].Start = (size_t) start;
			job->Groups [travel].End   = (size_t) taskId;
//            printf ("%3d/%3d %5d  %5d %5d\n",travel,job->GroupNum, job->Groups [travel].End - job->Groups [travel].Start, job->Groups [travel].Start,job->Groups [travel].End);
			travel = job->GroupNum - job->SortedTasks [taskId]->Travel - 1;
			start = taskId;
		}
	}
    job->Groups [travel].Start = (size_t) start;
    job->Groups [travel].End   = (size_t) taskId;
//    printf ("%3d/%3d %5d  %5d %5d\n",travel,job->GroupNum, job->Groups [travel].End - job->Groups [travel].Start, job->Groups [travel].Start,job->Groups [travel].End);
    job->Sorted = true;
	return (CMsucceeded);
}

void CMthreadJobDestroy (CMthreadJob_p job) {
	size_t group;

	if (job->Groups != (CMthreadTaskGroup_p) NULL) free (job->Groups);
	free (job->Tasks);
	free (job);
}

static void *_CMthreadWork (void *dataPtr) {
	CMthreadData_p data = (CMthreadData_p) dataPtr;
	size_t team, taskId, start, end;
	CMthreadCohort_p cohort = (CMthreadCohort_p) data->CohortPtr;
	CMthreadJob_p  job;
    struct timeb tbs;
    long long startTime;

    team = data->TeamID;
    pthread_mutex_lock (&(cohort->Teams[team].SMutex));
    do {
        pthread_cond_wait (&(cohort->Teams[team].SCond), &(cohort->Teams[team].SMutex));
        job = cohort->JobPtr;
        if (job != (CMthreadJob_p) NULL) {
            start = job->Groups[job->Group].Start;
            end   = job->Groups[job->Group].End;
            pthread_mutex_unlock(&(cohort->Teams[team].SMutex));
            ftime (&tbs);
            startTime = tbs.time * 1000 + tbs.millitm;
            for (taskId = start + data->ThreadID; taskId < end; taskId += cohort->Teams[team].ThreadNum)
                job->UserFunc(data->ThreadID, job->SortedTasks[taskId]->Id, job->CommonData);
            ftime (&tbs);
            data->Time += (tbs.time * 1000 + tbs.millitm - startTime);
            pthread_mutex_lock (&(cohort->Teams[team].SMutex));
            job->Completed++;
            if (job->Completed == cohort->Teams[team].ThreadNum) {
                pthread_mutex_lock   (&(cohort->Teams[team].MMutex));
                pthread_cond_signal  (&(cohort->Teams[team].MCond));
                pthread_mutex_unlock (&(cohort->Teams[team].MMutex));
            }
        }
    } while (job != (CMthreadJob_p) NULL);
    pthread_mutex_unlock (&(cohort->Teams[team].SMutex));
    pthread_exit ((void *) NULL);
}

CMreturn CMthreadJobExecute (CMthreadCohort_p cohort, CMthreadJob_p job) {
    int taskId, team;
    struct timeb tbs;
    long long startTime, localStart;

    ftime (&tbs);
    startTime = tbs.time * 1000 + tbs.millitm;

    if (job->Sorted == false) {
        _CMthreadJobTaskSort(job);
        ftime (&tbs);
        cohort->ExecTime += (tbs.time * 1000 + tbs.millitm - startTime);
    }
    ftime (&tbs);
    startTime = tbs.time * 1000 + tbs.millitm;

    if (cohort->ThreadNum < 1) {
        ftime (&tbs);
        localStart = tbs.time * 1000 + tbs.millitm;
        for (job->Group = 0; job->Group < job->GroupNum; job->Group++) {
            for (taskId = job->Groups[job->Group].Start; taskId < job->Groups[job->Group].End; ++taskId)
                job->UserFunc(0, job->SortedTasks[taskId]->Id, job->CommonData);
        }
        ftime (&tbs);
        cohort->Time += (tbs.time * 1000 + tbs.millitm - localStart);
    }
    else {
        for (job->Group = 0; job->Group < job->GroupNum; job->Group++) {
            for (team = 0; team < cohort->TeamNum; team++) {
                if (job->Groups[job->Group].End - job->Groups[job->Group].Start >= cohort->Teams[team].ThreadNum) {
                    pthread_mutex_lock(&(cohort->Teams[team].SMutex));
                    job->Completed = 0;
                    cohort->JobPtr = (void *) job;
                    pthread_cond_broadcast(&(cohort->Teams[team].SCond));
                    pthread_mutex_unlock(&(cohort->Teams[team].SMutex));
                    pthread_cond_wait(&(cohort->Teams[team].MCond), &(cohort->Teams[team].MMutex));
                    break;
                }
            }
        }
    }
    ftime (&tbs);
    cohort->ExecTime += (tbs.time * 1000 + tbs.millitm - startTime);
	return (CMsucceeded);
}

CMthreadCohort_p CMthreadCohortInitialize (CMthreadCohort_p cohort, size_t threadNum) {
    int ret, team;
	size_t threadId;
    pthread_attr_t thread_attr;
    struct timeb tbs;

    ftime (&tbs);

    for (team = 0;(0x01 << team) < threadNum; ++team);

    cohort->TotTime    = tbs.time * 1000 + tbs.millitm;
    cohort->TeamNum    = team + 1;
    cohort->ThreadNum  = threadNum;
	cohort->JobPtr     = (void *) NULL;
    cohort->ExecTime   = 0;
    cohort->ThreadTime = 0;
    cohort->Time       = 0;

    if (threadNum > 0) {
        if ((cohort->Teams = (CMthreadTeam_p) calloc (cohort->TeamNum, sizeof(CMthreadTeam_t))) == (CMthreadTeam_p) NULL) {
            CMmsgPrint (CMmsgSysError,"Memory Allocation error in %s:%d",__FILE__,__LINE__);
            free (cohort);
            return ((CMthreadCohort_p) NULL);
        }
        for (team = 0;team < cohort->TeamNum; ++team) {
            cohort->Teams [team].ThreadNum = team > 0 ? 0x01 << (cohort->TeamNum - team) : threadNum;
            if ((cohort->Teams [team].Threads = (CMthreadData_p) calloc (cohort->Teams[team].ThreadNum, sizeof(CMthreadData_t))) == (CMthreadData_p) NULL) {
                CMmsgPrint (CMmsgSysError,"Memory Allocation error in %s:%d",__FILE__,__LINE__);
                cohort->TeamNum = team;
                CMthreadTeamDestroy (cohort);
                return ((CMthreadCohort_p) NULL);
            }

            pthread_attr_init (&thread_attr);
            pthread_attr_setdetachstate (&thread_attr, PTHREAD_CREATE_JOINABLE);
            pthread_attr_setscope       (&thread_attr, PTHREAD_SCOPE_SYSTEM);

            pthread_mutex_init (&(cohort->Teams [team].MMutex), NULL);
            pthread_cond_init  (&(cohort->Teams [team].MCond),  NULL);
            pthread_mutex_init (&(cohort->Teams [team].SMutex), NULL);
            pthread_cond_init  (&(cohort->Teams [team].SCond),  NULL);
            for (threadId = 0; threadId < cohort->Teams [team].ThreadNum; ++threadId) {
                cohort->Teams [team].Threads[threadId].ThreadID  = threadId;
                cohort->Teams [team].Threads[threadId].TeamID    = team;
                cohort->Teams [team].Threads[threadId].CohortPtr = (void *) cohort;
                cohort->Teams [team].Threads[threadId].Time      = 0;
                if ((ret = pthread_create(&(cohort->Teams [team].Threads[threadId].Thread), &thread_attr, _CMthreadWork,
                                          (void *) (cohort->Teams [team].Threads + threadId))) != 0) {
                    CMmsgPrint(CMmsgSysError, "Thread creation returned with error [%d] in %s:%d", ret, __FILE__, __LINE__);
                        CMthreadTeamDestroy (cohort);
                    return ((CMthreadCohort_p) NULL);
                }
                while (pthread_kill(cohort->Teams [team].Threads[threadId].Thread,0) != 0); // TODO this might turn out to be sloppy
            }
            pthread_attr_destroy(&thread_attr);
            pthread_mutex_lock (&(cohort->Teams [team].MMutex));
        }
    }
	return (cohort);
}

void CMthreadTeamDestroy (CMthreadCohort_p cohort) { // Does not free the team pointer so
    size_t threadId, team;
    void *status;
    struct timeb tbs;

    for (team = 0; team < cohort->TeamNum; ++team) {
        if (cohort->Teams [team].ThreadNum > 0) {
            cohort->JobPtr = (CMthreadJob_p) NULL;
            pthread_mutex_lock     (&(cohort->Teams [team].SMutex));
            pthread_cond_broadcast (&(cohort->Teams [team].SCond));
            pthread_mutex_unlock   (&(cohort->Teams [team].SMutex));
            for (threadId = 0; threadId < cohort->Teams [team].ThreadNum; ++threadId) {
                pthread_join(cohort->Teams [team].Threads[threadId].Thread, &status);
                cohort->ThreadTime += cohort->Teams [team].Threads[threadId].Time;
            }
            pthread_mutex_unlock (&(cohort->Teams [team].MMutex));
            pthread_mutex_destroy(&(cohort->Teams [team].MMutex));
            pthread_mutex_destroy(&(cohort->Teams [team].SMutex));
            pthread_cond_destroy (&(cohort->Teams [team].SCond));
            free(cohort->Teams [team].Threads);
        }
    }
    ftime (&tbs);
    cohort->TotTime = tbs.time * 1000 + tbs.millitm - cohort->TotTime;
}
