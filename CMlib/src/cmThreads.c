/******************************************************************************

GHAAS Command Line Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2016, UNH - CCNY/CUNY

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
	size_t taskId, threadId, threadNum;
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
		job->SortedTasks [taskId]       = job->Tasks + taskId;
		job->Tasks [taskId].Id          = taskId;
		job->Tasks [taskId].Dependent   = (CMthreadTask_p) NULL;
		job->Tasks [taskId].DependLevel = 0;
	}
	job->UserFunc = execFunc;
	job->CommonData = (void *) commonData;
	return (job);
}

CMreturn CMthreadJobTaskDependent (CMthreadJob_p job, size_t taskId, size_t dependent) {
	if (taskId    > job->TaskNum) {
		CMmsgPrint (CMmsgAppError,"Invalid task in %s%d",__FILE__,__LINE__);
		return (CMfailed);
	}
	if (dependent > job->TaskNum) {
		CMmsgPrint (CMmsgAppError,"Invalid dependence in %s%d",__FILE__,__LINE__);
		return (CMfailed);
	}
	if (taskId == dependent) return (CMsucceeded);
	job->Tasks [taskId].Dependent = job->Tasks + dependent;
    job->Sorted = false;
	return (CMsucceeded);
}

static int _CMthreadJobTaskCompare (const void *lPtr,const void *rPtr) {
	int ret;
	CMthreadTask_p lTaskInit = *((CMthreadTask_p *) lPtr);
	CMthreadTask_p rTaskInit = *((CMthreadTask_p *) rPtr);
	CMthreadTask_p lTask = lTaskInit;
	CMthreadTask_p rTask = rTaskInit;

	if ((ret = (int) rTask->DependLevel - (int) lTask->DependLevel) != 0) return (ret);

	while (((rTask = rTask->Dependent) != (CMthreadTask_p) NULL) &&
           ((lTask = lTask->Dependent) != (CMthreadTask_p) NULL)) {
		if ((ret = (int) rTask->DependLevel - (int) lTask->DependLevel) != 0) return (ret);
	}
	if (rTask == lTask) return (0);
	if (rTask != (CMthreadTask_p) NULL) return (1);
	return (-1);
}

CMreturn _CMthreadJobTaskSort (CMthreadJob_p job) {
	size_t taskId, start;
	size_t level;
	CMthreadTask_p dependent;

	for (taskId = 0;taskId < job->TaskNum; ++taskId) {
		level = 0;
        for (dependent = job->Tasks + taskId; dependent->Dependent != (CMthreadTask_p) NULL; dependent = dependent->Dependent) {
            level++;
		}
        job->Tasks [taskId].DependLevel = level;
	}
    qsort (job->SortedTasks,job->TaskNum,sizeof (CMthreadTask_p),_CMthreadJobTaskCompare);
	job->GroupNum = job->SortedTasks [0]->DependLevel + 1;
	if ((job->Groups = (CMthreadTaskGroup_p) realloc (job->Groups, job->GroupNum * sizeof (CMthreadTaskGroup_t))) == (CMthreadTaskGroup_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s:%d",__FILE__,__LINE__);
		return (CMfailed);
	}
	level = 0;
	start = 0;
	for (taskId = 0;taskId < job->TaskNum; ++taskId) {
		if (level != job->GroupNum - job->SortedTasks [taskId]->DependLevel - 1) {
			job->Groups [level].Start = (size_t) start;
			job->Groups [level].End   = (size_t) taskId;
//            printf ("%3d/%3d %5d  %5d %5d\n",level,job->GroupNum, job->Groups [level].End - job->Groups [level].Start, job->Groups [level].Start,job->Groups [level].End);
			level = job->GroupNum - job->SortedTasks [taskId]->DependLevel - 1;
			start = taskId;
		}
	}
    job->Groups [level].Start = (size_t) start;
    job->Groups [level].End   = (size_t) taskId;
//    printf ("%3d/%3d %5d  %5d %5d\n",level,job->GroupNum, job->Groups [level].End - job->Groups [level].Start, job->Groups [level].Start,job->Groups [level].End);
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
	size_t taskId, start, end;
	CMthreadTeam_p team = (CMthreadTeam_p) data->TeamPtr;
	CMthreadJob_p  job;
    struct timeb tbs;
    long long startTime;

    pthread_mutex_lock (&(team->SMutex));
    do {
        pthread_cond_wait (&(team->SCond), &(team->SMutex));
        job = team->JobPtr;
        if (job != (CMthreadJob_p) NULL) {
            start = job->Groups[job->Group].Start;
            end   = job->Groups[job->Group].End;
            pthread_mutex_unlock(&(team->SMutex));
            ftime (&tbs);
            startTime = tbs.time * 1000 + tbs.millitm;
            for (taskId = start + data->Id; taskId < end; taskId += team->ThreadNum)
                job->UserFunc(data->Id, job->SortedTasks[taskId]->Id, job->CommonData);
            ftime (&tbs);
            data->Time += (tbs.time * 1000 + tbs.millitm - startTime);
            pthread_mutex_lock (&(team->SMutex));
            job->Completed++;
            if (job->Completed == team->ThreadNum) {
                pthread_mutex_lock   (&(team->MMutex));
                pthread_cond_signal  (&(team->MCond));
                pthread_mutex_unlock (&(team->MMutex));
            }
        }
    } while (job != (CMthreadJob_p) NULL);
    pthread_mutex_unlock (&(team->SMutex));
    pthread_exit ((void *) NULL);
}

CMreturn CMthreadJobExecute (CMthreadTeam_p team, CMthreadJob_p job) {
    int taskId;
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
            # pragma opm parallel for
            for (taskId = job->Groups[job->Group].Start; taskId < job->Groups[job->Group].End; ++taskId)
                job->UserFunc(0, job->SortedTasks[taskId]->Id, job->CommonData);
        }
        ftime (&tbs);
        team->Time += (tbs.time * 1000 + tbs.millitm - localStart);
    }
    else {
        for (job->Group = 0; job->Group < job->GroupNum; job->Group++) {
            if (job->Groups[job->Group].End - job->Groups[job->Group].Start < team->ThreadNum * 2) {
                ftime (&tbs);
                localStart = tbs.time * 1000 + tbs.millitm;
                for (taskId = job->Groups[job->Group].Start; taskId < job->Groups[job->Group].End; ++taskId)
                    job->UserFunc(0, job->SortedTasks[taskId]->Id, job->CommonData);
                team->Time += (tbs.time * 1000 + tbs.millitm - localStart);
            }
            else {
                pthread_mutex_lock     (&(team->SMutex));
                job->Completed = 0;
                team->JobPtr = (void *) job;
                pthread_cond_broadcast (&(team->SCond));
                pthread_mutex_unlock   (&(team->SMutex));
                pthread_cond_wait (&(team->MCond), &(team->MMutex));
            }
        }
    }
    ftime (&tbs);
    team->ExecTime += (tbs.time * 1000 + tbs.millitm - startTime);
	return (CMsucceeded);
}

CMthreadTeam_p CMthreadTeamInitialize (CMthreadTeam_p team, size_t threadNum) {
    int ret;
	size_t threadId;
    pthread_attr_t thread_attr;
    struct timeb tbs;

    ftime (&tbs);
    team->TotTime = tbs.time * 1000 + tbs.millitm;
	team->ThreadNum      = threadNum;
	team->JobPtr         = (void *) NULL;
    team->ExecTime       = 0;
    team->Time           = 0;

    if (team->ThreadNum > 0) {
        if ((team->Threads = (CMthreadData_p) calloc (threadNum, sizeof(CMthreadData_t))) == (CMthreadData_p) NULL) {
            CMmsgPrint (CMmsgSysError,"Memory Allocation error in %s:%d",__FILE__,__LINE__);
            free (team);
            return ((CMthreadTeam_p) NULL);
        }
        pthread_attr_init (&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

        pthread_mutex_init (&(team->MMutex), NULL);
        pthread_cond_init  (&(team->MCond),  NULL);
        pthread_mutex_init (&(team->SMutex), NULL);
        pthread_cond_init  (&(team->SCond),  NULL);
        for (threadId = 0; threadId < team->ThreadNum; ++threadId) {
            team->Threads[threadId].Id      = threadId;
            team->Threads[threadId].TeamPtr = (void *) team;
            team->Threads[threadId].Time    = 0;
            if ((ret = pthread_create(&(team->Threads[threadId].Thread), &thread_attr, _CMthreadWork,
                                      (void *) (team->Threads + threadId))) != 0) {
                CMmsgPrint(CMmsgSysError, "Thread creation returned with error [%d] in %s:%d", ret, __FILE__, __LINE__);
                free(team->Threads);
                free(team);
                return ((CMthreadTeam_p) NULL);
            }
            while (pthread_kill(team->Threads[threadId].Thread,0) != 0); // TODO this might turn out to be sloppy
        }
        pthread_attr_destroy(&thread_attr);
        pthread_mutex_lock (&(team->MMutex));
    }
	return (team);
}

void CMthreadTeamDestroy (CMthreadTeam_p team) { // Does not free the team pointer so
    size_t threadId;
    void *status;
    struct timeb tbs;

    if (team->ThreadNum > 0) {
        team->JobPtr = (CMthreadJob_p) NULL;
        pthread_mutex_lock     (&(team->SMutex));
        pthread_cond_broadcast (&(team->SCond));
        pthread_mutex_unlock   (&(team->SMutex));
        for (threadId = 0; threadId < team->ThreadNum; ++threadId) {
            pthread_join(team->Threads[threadId].Thread, &status);
            team->Time += team->Threads[threadId].Time;
            CMmsgPrint (CMmsgInfo,"Thread [%d]:%.1f",threadId, (float) team->Threads[threadId].Time / 1000.0);
        }
        pthread_mutex_unlock (&(team->MMutex));
        pthread_mutex_destroy(&(team->MMutex));
        pthread_mutex_destroy(&(team->SMutex));
        pthread_cond_destroy (&(team->SCond));
        free(team->Threads);
   }
    ftime (&tbs);
    team->TotTime = tbs.time * 1000 + tbs.millitm - team->TotTime;
}