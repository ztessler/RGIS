/******************************************************************************

GHAAS Command Line Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2019, UNH - CUNY

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
	size_t group, taskId, start, end;
	CMthreadTeam_p team = (CMthreadTeam_p) data->TeamPtr;
	CMthreadJob_p  job;
    struct timeb tbs;
    long long startTime;

    group = team->Group;
    pthread_mutex_lock (&(team->Groups[group].SMutex));
    do {
        pthread_cond_wait (&(team->Groups[group].SCond), &(team->Groups[group].SMutex));
        job = team->JobPtr;
        if (job != (CMthreadJob_p) NULL) {
            start = job->Groups[group].Start;
            end   = job->Groups[group].End;
            pthread_mutex_unlock(&(team->Groups[group].SMutex));
            ftime (&tbs);
            startTime = tbs.time * 1000 + tbs.millitm;
            for (taskId = start + data->Id; taskId < end; taskId += team->ThreadNum)
                job->UserFunc(data->Id, job->SortedTasks[taskId]->Id, job->CommonData);
            ftime (&tbs);
            data->Time += (tbs.time * 1000 + tbs.millitm - startTime);
            pthread_mutex_lock (&(team->Groups[group].SMutex));
            job->Completed++;
            if (job->Completed == team->Groups[group].ThreadNum) {
                pthread_mutex_lock   (&(team->Groups[group].MMutex));
                pthread_cond_signal  (&(team->Groups[group].MCond));
                pthread_mutex_unlock (&(team->Groups[group].MMutex));
            }
        }
    } while (job != (CMthreadJob_p) NULL);
    pthread_mutex_unlock (&(team->Groups[group].SMutex));
    pthread_exit ((void *) NULL);
}

CMreturn CMthreadJobExecute (CMthreadTeam_p team, CMthreadJob_p job) {
    int taskId, group;
    struct timeb tbs;
    long long startTime, localStart;

    ftime (&tbs);
    startTime = tbs.time * 1000 + tbs.millitm;

    if (job->Sorted == false) {
        _CMthreadJobTaskSort(job);
        ftime (&tbs);
        team->ExecTime += (tbs.time * 1000 + tbs.millitm - startTime);
    }
    ftime (&tbs);
    startTime = tbs.time * 1000 + tbs.millitm;

    if (team->ThreadNum < 1) {
        ftime (&tbs);
        localStart = tbs.time * 1000 + tbs.millitm;
        for (job->Group = 0; job->Group < job->GroupNum; job->Group++) {
            for (taskId = job->Groups[job->Group].Start; taskId < job->Groups[job->Group].End; ++taskId)
                job->UserFunc(0, job->SortedTasks[taskId]->Id, job->CommonData);
        }
        ftime (&tbs);
        team->Time += (tbs.time * 1000 + tbs.millitm - localStart);
    }
    else {
        for (job->Group = 0; job->Group < job->GroupNum; job->Group++) {
            for (group = team->GroupNum; group >= 0; --group) {
                if (job->Groups[job->Group].End - job->Groups[job->Group].Start > team->Groups [group].ThreadNum * 0x40) {
                    pthread_mutex_lock     (&(team->Groups [group].SMutex));
                    job->Completed = 0;
                    team->JobPtr = (void *) job;
                    pthread_cond_broadcast (&(team->Groups [group].SCond));
                    pthread_mutex_unlock   (&(team->Groups [group].SMutex));
                    pthread_cond_wait (&(team->Groups [group].MCond), &(team->Groups [group].MMutex));
                    break;
                }
            }
            if (group >= 0) {
                ftime(&tbs);
                localStart = tbs.time * 1000 + tbs.millitm;
                for (taskId = job->Groups[job->Group].Start; taskId < job->Groups[job->Group].End; ++taskId)
                    job->UserFunc(0, job->SortedTasks[taskId]->Id, job->CommonData);
                ftime(&tbs);
                team->Time += (tbs.time * 1000 + tbs.millitm - localStart);
            }
        }
    }
    ftime (&tbs);
    team->ExecTime += (tbs.time * 1000 + tbs.millitm - startTime);
	return (CMsucceeded);
}

CMthreadTeam_p CMthreadTeamInitialize (CMthreadTeam_p team, size_t threadNum) {
    int ret, group;
	size_t threadId;
    pthread_attr_t thread_attr;
    struct timeb tbs;

    ftime (&tbs);
    team->TotTime = tbs.time * 1000 + tbs.millitm;
	team->ThreadNum      = threadNum;
	team->JobPtr         = (void *) NULL;
    team->ExecTime       = 0;
    team->ThreadTime     = 0;
    team->Time           = 0;

    if (team->ThreadNum > 0) {
        for (group = 0; (0x01 << group) < threadNum; ++group);
        team->GroupNum = group;
        if ((team->Groups = (CMthreadGroup_p) calloc (team->GroupNum, sizeof(CMthreadGroup_t))) == (CMthreadGroup_p) NULL) {
            CMmsgPrint (CMmsgSysError,"Memory Allocation error in %s:%d",__FILE__,__LINE__);
            free (team);
            return ((CMthreadTeam_p) NULL);
        }
        for (group = 0; group <= team->GroupNum; ++group) {
            team->Group = team->Groups [group].Id = group;
            if ((team->Groups [group].Threads = (CMthreadData_p) calloc (team->GroupNum + 1, sizeof(CMthreadData_t))) == (CMthreadData_p) NULL) {
                CMmsgPrint (CMmsgSysError,"Memory Allocation error in %s:%d",__FILE__,__LINE__);
                free (team);
                return ((CMthreadTeam_p) NULL);
            }
            pthread_attr_init (&thread_attr);
            pthread_attr_setdetachstate (&thread_attr, PTHREAD_CREATE_JOINABLE);
            pthread_attr_setscope       (&thread_attr, PTHREAD_SCOPE_SYSTEM);
            pthread_mutex_init (&(team->Groups [group].MMutex), NULL);
            pthread_cond_init  (&(team->Groups [group].MCond),  NULL);
            pthread_mutex_init (&(team->Groups [group].SMutex), NULL);
            pthread_cond_init  (&(team->Groups [group].SCond),  NULL);
            team->Groups [group].ThreadNum = group < team->GroupNum ? 0x01 << group : team->ThreadNum;
            for (threadId = 0; threadId < team->Groups [group].ThreadNum; ++threadId) {
                team->Groups [group].Threads[threadId].Id      = threadId;
                team->Groups [group].Threads[threadId].TeamPtr = (void *) team;
                team->Groups [group].Threads[threadId].Time    = 0;
                if ((ret = pthread_create(&(team->Groups [group].Threads[threadId].Thread), &thread_attr, _CMthreadWork,
                                          (void *) (team->Groups [group].Threads + threadId))) != 0) {
                    CMmsgPrint(CMmsgSysError, "Thread creation returned with error [%d] in %s:%d", ret, __FILE__, __LINE__);
                    for ( ; group >= 0; --group) free(team->Groups [group].Threads);
                    free (team->Groups);
                    free(team);
                    return ((CMthreadTeam_p) NULL);
                }
                while (pthread_kill (team->Groups [group].Threads[threadId].Thread, 0) != 0); // TODO this might turn out to be sloppy
            }
            pthread_attr_destroy(&thread_attr);
            pthread_mutex_lock (&(team->Groups [group].MMutex));
        }
    }
	return (team);
}

void CMthreadTeamDestroy (CMthreadTeam_p team) { // Does not free the team pointer so
    int group;
    size_t threadId;
    void *status;
    struct timeb tbs;

    if (team->ThreadNum > 0) {
        for (group = 0; group < team->GroupNum; ++group) {
            team->JobPtr = (CMthreadJob_p) NULL;
            pthread_mutex_lock     (&(team->Groups[group].SMutex));
            pthread_cond_broadcast (&(team->Groups[group].SCond));
            pthread_mutex_unlock   (&(team->Groups[group].SMutex));
            for (threadId = 0; threadId < team->Groups [group].ThreadNum; ++threadId) {
                pthread_join(team->Groups [group].Threads[threadId].Thread, &status);
                team->ThreadTime += team->Groups [group].Threads[threadId].Time;
            }
            pthread_mutex_unlock (&(team->Groups [group].MMutex));
            pthread_mutex_destroy(&(team->Groups [group].MMutex));
            pthread_mutex_destroy(&(team->Groups [group].SMutex));
            pthread_cond_destroy (&(team->Groups [group].SCond));
            free(team->Groups [group].Threads);
        }
        free (team->Groups);
    }
    ftime (&tbs);
    team->TotTime = tbs.time * 1000 + tbs.millitm - team->TotTime;
}
