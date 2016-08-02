/******************************************************************************

GHAAS Command Line Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2016, UNH - CCNY/CUNY

cmThreads.c

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <cm.h>

size_t CMthreadProcessorNum () {
	char *procEnv;
	int procNum;

	if (((procEnv = getenv ("GHAASprocessorNum")) != (char *) NULL) && (sscanf (procEnv,"%d",&procNum)))
		return (procNum);
	return (0);
}

static CMreturn _CMthreadTaskGroupInitialize (CMthreadTaskGroup_p group, size_t id, size_t threadNum, size_t taskNum, size_t start) {
	size_t threadId, pos, num;

	if ((group->Start = (size_t *) realloc (group->Start, threadNum * sizeof (size_t))) == (size_t *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s %d\n",__FILE__,__LINE__);
		return (CMfailed);
	}
	if ((group->End  = (size_t *) realloc  (group->End, threadNum * sizeof (size_t))) == (size_t *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s %d\n",__FILE__,__LINE__);
		free (group->Start);
		group->Start = (size_t *) NULL;
		return (CMfailed);
	}
    if (taskNum > 2 * threadNum) {
        pos = start;
        num   = taskNum > threadNum ? (int) (ceil ((float) taskNum / (float) threadNum)) : 1;
        for (threadId = 0;threadId < threadNum; threadId++) {
            if (start + taskNum < pos + num) num = start + taskNum - pos;
            group->Start [threadId] = pos;
            group->End   [threadId] = pos + num;
            pos = pos + num;
        }
    }
    else {
        group->Start [0] = start;
        group->End   [0] = start + taskNum;
        for (threadId = 1;threadId < threadNum; threadId++) {
            group->Start [threadId] = pos + taskNum;
            group->End   [threadId] = pos + taskNum;
        }
    }
	return (CMsucceeded);
}

CMthreadJob_p CMthreadJobCreate (CMthreadTeam_p team,
		                         void *commonData,
		                         size_t taskNum,
		                         CMthreadUserExecFunc  execFunc) {
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
    job->Groups [0].Start = (size_t *) NULL;
    job->Groups [0].End   = (size_t *) NULL;
	threadNum = (team != (CMthreadTeam_p) NULL) ? team->ThreadNum : 1;
	if (_CMthreadTaskGroupInitialize (job->Groups + 0, 0, threadNum, taskNum, 0) != CMsucceeded) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d",__FILE__,__LINE__);
		free (job->Groups);
		free (job->SortedTasks);
		free (job->Tasks);
		free (job);
		return ((CMthreadJob_p) NULL);
	}
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
		job->Tasks [taskId].DependNum   = 0;
	}
	job->UserFunc = execFunc;
	job->CommonData = (void *) commonData;
	
	if ((job->Data = (void **) calloc (threadNum, sizeof (void *))) == (void **) NULL) {
		CMmsgPrint (CMmsgSysError, "Memory allocation error in %s:%d",__FILE__,__LINE__);
		free (job);
		return ((CMthreadJob_p) NULL);
		}
	for (threadId = 0;threadId < threadNum;++threadId) job->Data [threadId] = (void *) NULL;
	return (job);
}

CMreturn CMthreadJobTaskDependent (CMthreadJob_p job, size_t taskId, size_t dependent) {
	job->Sorted = false;

	if (taskId    > job->TaskNum) {
		CMmsgPrint (CMmsgAppError,"Invalid task in %s%d",__FILE__,__LINE__);
		return (CMfailed);
	}
	if (dependent > job->TaskNum) {
		CMmsgPrint (CMmsgAppError,"Invalid dependence in %s%d",__FILE__,__LINE__);
		return (CMfailed);
	}
	if (taskId == dependent) return (CMsucceeded);

	if (taskId < dependent) job->Tasks [taskId].Dependent = job->Tasks + dependent;
	else CMmsgPrint (CMmsgWarning,"Invalid dependence [%d:%d] ignored!", dependent, taskId);
	return (CMsucceeded);
}

static int _CMthreadJobTaskCompare (const void *lPtr,const void *rPtr) {
	int ret;
	CMthreadTask_p lTaskInit = *((CMthreadTask_p *) lPtr);
	CMthreadTask_p rTaskInit = *((CMthreadTask_p *) rPtr);
	CMthreadTask_p lTask = lTaskInit;
	CMthreadTask_p rTask = rTaskInit;

	if ((ret = lTask->DependLevel - rTask->DependLevel) != 0) return (ret);

	while (((lTask = lTask->Dependent) != (CMthreadTask_p) NULL) &&
	       ((rTask = rTask->Dependent) != (CMthreadTask_p) NULL)) {
		if ((ret = lTask->DependLevel - rTask->DependLevel) != 0) return (ret);
	}
	if (lTask == rTask) return (0);
	if (lTask != (CMthreadTask_p) NULL) return (1);
	return (-1);
}

CMreturn _CMthreadJobTaskSort (CMthreadJob_p job, int threadNum) {
	size_t taskId, start;
	size_t group;
	CMthreadTask_p dependent;

	for (taskId = 0;taskId < job->TaskNum; ++taskId) {
		group = 1;
		if (job->Tasks [taskId].Dependent != (CMthreadTask_p) NULL) job->Tasks [taskId].Dependent->DependNum++;
		for (dependent = job->Tasks + taskId; dependent->Dependent != (CMthreadTask_p) NULL; dependent = dependent->Dependent) {
			if (dependent->Dependent->DependLevel < group) {
				dependent->Dependent->DependLevel = group;
				group++;
			}
			else break;
		}
	}
	qsort (job->SortedTasks,job->TaskNum,sizeof (CMthreadTask_p),_CMthreadJobTaskCompare);
	job->GroupNum = job->SortedTasks [job->TaskNum - 1]->DependLevel;
	if ((job->Groups = (CMthreadTaskGroup_p) realloc (job->Groups, job->GroupNum * sizeof (CMthreadTaskGroup_t))) == (CMthreadTaskGroup_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory allocation error in: %s:%d",__FILE__,__LINE__);
		return (CMfailed);
	}
	group = 0;
	start = 0;
	for (taskId = 0;taskId < job->TaskNum; ++taskId) {
		if (group != job->SortedTasks [taskId]->DependLevel) {
            if (group > 0) {
                job->Groups [group].Start = (size_t *) NULL;
                job->Groups [group].End   = (size_t *) NULL;
            }
			if (_CMthreadTaskGroupInitialize (job->Groups + group, group, threadNum, taskId - start, start) != CMsucceeded) {
				CMmsgPrint (CMmsgAppError,"Task group initialization error in: %s:%d",__FILE__,__LINE__);
				return (CMfailed);
			}
			group = job->SortedTasks [taskId]->DependLevel;
			start = taskId;
		}
	}
	return (CMsucceeded);
}

void CMthreadJobDestroy (CMthreadJob_p job) {
	size_t group;

	for (group = 0; group < job->GroupNum; group++) {
		if (job->Groups [group].Start != (size_t *) NULL) free (job->Groups [group].Start);
		if (job->Groups [group].End   != (size_t *) NULL) free (job->Groups [group].End);
	}
	if (job->Groups != (CMthreadTaskGroup_p) NULL) free (job->Groups);
	free (job->Tasks);
	free (job->Data);
	free (job);
}

static void *_CMthreadWork (void *dataPtr) {
	CMthreadData_p data = (CMthreadData_p) dataPtr;
	size_t taskId, start, end;
	CMthreadTeam_p team = (CMthreadTeam_p) data->TeamPtr;
	CMthreadJob_p  job;

    pthread_mutex_lock (&(team->Mutex));
    do {
        usleep(1);
        pthread_cond_wait (&(team->Cond), &(team->Mutex));
        job = team->JobPtr;
        if (job != (CMthreadJob_p) NULL) {
            start = job->Groups[job->Group].Start [data->Id];
            end   = job->Groups[job->Group].End   [data->Id];
            pthread_mutex_unlock(&(team->Mutex));
            for (taskId = start; taskId < end; taskId++)
                job->UserFunc(job->CommonData, job->Data[data->Id], job->SortedTasks[taskId]->Id);
            pthread_mutex_lock (&(team->Mutex));
            job->Completed++;
            if (job->Completed == team->ThreadNum) pthread_cond_signal (&(team->Cond));
        }
    } while (job != (CMthreadJob_p) NULL);
    pthread_mutex_unlock (&(team->Mutex));
    pthread_exit ((void *) NULL);
}

CMreturn CMthreadJobExecute (CMthreadTeam_p team, CMthreadJob_p job) {
    int taskId, threadId;

    if (job->Sorted == false) { _CMthreadJobTaskSort(job,team == (CMthreadTeam_p) NULL ? 1 : team->ThreadNum ); job->Sorted = true; }

    if ((team == (CMthreadTeam_p) NULL) || (team->ThreadNum < 2)) {
        for (job->Group = 0; job->Group < job->GroupNum; job->Group++) {
            for (taskId = job->Groups[job->Group].Start[0]; taskId < job->Groups[job->Group].End[0]; ++taskId)
                job->UserFunc(job->CommonData, job->Data, taskId);
        }
    }
    else {
        team->JobPtr = (void *) job;
        for (job->Group = 0; job->Group < job->GroupNum; job->Group++) {
            job->Completed = 0;
            pthread_cond_broadcast (&(team->Cond));
            usleep(1);
            pthread_cond_wait (&(team->Cond), &(team->Mutex));
        }
    }
	return (CMsucceeded);
}

CMthreadTeam_p CMthreadTeamCreate (size_t threadNum) {
    int ret;
	size_t threadId;
    pthread_attr_t thread_attr;
	CMthreadTeam_p team;

    if (threadNum < 1) return ((CMthreadTeam_p) NULL);

	if ((team = (CMthreadTeam_p) malloc (sizeof (CMthreadTeam_t))) == (CMthreadTeam_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation error in %s:%d",__FILE__,__LINE__);
		return ((CMthreadTeam_p) NULL);
	}
	if ((team->Threads = (CMthreadData_p) calloc (threadNum, sizeof(CMthreadData_t))) == (CMthreadData_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation error in %s:%d",__FILE__,__LINE__);
		free (team);
		return ((CMthreadTeam_p) NULL);
	}
	team->ThreadNum      = threadNum;
	team->JobPtr         = (void *) NULL;
	team->Time           = clock ();

    pthread_attr_init (&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

    if (team->ThreadNum > 1) {
        pthread_mutex_init (&(team->Mutex), NULL);
        pthread_cond_init  (&(team->Cond),  NULL);
        for (threadId = 0; threadId < team->ThreadNum; ++threadId) {
            team->Threads[threadId].Id = threadId;
            team->Threads[threadId].TeamPtr = (void *) team;
            if ((ret = pthread_create(&(team->Threads[threadId].Thread), &thread_attr, _CMthreadWork,
                                      (void *) (team->Threads + threadId))) != 0) {
                CMmsgPrint(CMmsgSysError, "Thread creation returned with error [%d] in %s:%d", ret, __FILE__, __LINE__);
                free(team->Threads);
                free(team);
                return ((CMthreadTeam_p) NULL);
            }
        }
        pthread_attr_destroy(&thread_attr);
        usleep(1);
        pthread_mutex_lock (&(team->Mutex));
    }
	return (team);
}

void CMthreadTeamDestroy (CMthreadTeam_p team) {
    size_t threadId;
    void *status;

    team->JobPtr = (CMthreadJob_p) NULL;
    team->Time = clock() - team->Time;
    if (team->ThreadNum > 1) {
        pthread_mutex_unlock   (&(team->Mutex));
        pthread_cond_broadcast (&(team->Cond));
/*        for (threadId = 0; threadId < team->ThreadNum; ++threadId) {
            pthread_join(team->Threads[threadId].Thread, &status);
        }
*/
        pthread_mutex_destroy(&(team->Mutex));
        pthread_cond_destroy(&(team->Cond));
    }
    free(team->Threads);
    free(team);
}