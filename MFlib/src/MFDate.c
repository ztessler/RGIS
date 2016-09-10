/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2016, UNH - CCNY/CUNY

MFDate.c

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>

static struct MFDate_s {
	int Year;
	int Month;
	int Day;
	int DayOfYear;
} _MFCurrentDate;

static int _MFDateLeapYear (int year) {
    int leapYear = 0;

    if (year == MFDefaultMissingInt) return (0);

    if ((year & 0x03) == 0x00) leapYear = 1;
    if ((year % 100)  == 0) {
        if (((year / 100) & 0x03) == 0) leapYear = 1;
        else  leapYear = 0;
    }
    return (leapYear);
}

static int _MFDateMonthLength (int year,int month) {
    int nDays [] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if ((month < 1) || (month > 12)) return (MFDefaultMissingInt);

    return (nDays [month - 1] + (month == 2 ? _MFDateLeapYear (year) : 0));
}

int MFDateGetCurrentDay   () { return (_MFCurrentDate.Day); }
int MFDateGetCurrentMonth () { return (_MFCurrentDate.Month); }
int MFDateGetCurrentYear  () { return (_MFCurrentDate.Year); }
int MFDateGetDayOfYear    () { return (_MFCurrentDate.DayOfYear); }
int MFDateGetMonthLength  () { return (_MFDateMonthLength (_MFCurrentDate.Year,_MFCurrentDate.Month)); }

char *MFDateTimeStepString (int tStep) {
	switch (tStep) {
		case MFTimeStepYear:  return ("year");
		case MFTimeStepMonth: return ("month");
		case MFTimeStepDay:   return ("day");
		default: CMmsgPrint (CMmsgAppError, "Warning: Invalid time step [%d] in: %s %d",tStep,__FILE__,__LINE__); break;
	}
	return ("");
}

char *MFDateTimeStepUnit (int tStep) {
	switch (tStep) {
		case MFTimeStepYear:  return ("yr");
		case MFTimeStepMonth: return ("mo");
		case MFTimeStepDay:   return ("d");
        case MFTimeStepHour:  return ("hr");
		default: CMmsgPrint (CMmsgAppError,"Error: Invalid time step [%d] in: %s:%d",tStep,__FILE__,__LINE__); break;
	}
	return ("");
}

static bool _MFDateSet (struct MFDate_s *date,char *dateString) {
	int month;
	date->Year = date->Month = date->Day = MFDefaultMissingInt;
	if ((strncmp (dateString,MFDateClimatologyYearStr,strlen (MFDateClimatologyYearStr)) != 0) &&
		 (sscanf (dateString,"%d",&(date->Year)) != 1)) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid date [%s] in: %s:%d",dateString,__FILE__,__LINE__);
		return (false);
	}
	date->DayOfYear = 183;
	if (strlen (dateString) == 4) return (true);
	if (sscanf (dateString + 5,"%d",&(date->Month)) != 1) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid date [%s] in: %s:%d",dateString,__FILE__,__LINE__);
		return (false);
	}
	date->DayOfYear = 0;
	for (month = 1;month < date->Month;++month) date->DayOfYear += _MFDateMonthLength (date->Year,month);
	if (strlen (dateString) == 7) { date->DayOfYear +=_MFDateMonthLength (date->Year,month) / 2; return (true); }
	if (sscanf (dateString + 8,"%d",&(date->Day)) != 1) {
		CMmsgPrint (CMmsgAppError,"Error: Invalid date [%s] in: %s:%d",dateString,__FILE__,__LINE__);
		return (false);
	}
	date->DayOfYear += date->Day;
	return (true);
}

bool MFDateSetCurrent (char *currentDate) { return (_MFDateSet (&_MFCurrentDate,currentDate)); }

char *MFDateGetCurrent () {
    static char time [MFDateStringLength];

    if (_MFCurrentDate.Year != MFDefaultMissingInt) sprintf (time,"%4d",_MFCurrentDate.Year);
    else strcpy (time,MFDateClimatologyYearStr);

    if (_MFCurrentDate.Month   == MFDefaultMissingInt) return (time);
    sprintf (time + strlen (time),"-%02d",_MFCurrentDate.Month);
    if (_MFCurrentDate.Day     == MFDefaultMissingInt) return (time);
    sprintf (time + strlen (time),"-%02d",_MFCurrentDate.Day);
    return (time);
}

static int _MFDateCompare (struct MFDate_s *date0,struct MFDate_s *date1) {
	if ((date0->Year != MFDefaultMissingInt) && (date1->Year != MFDefaultMissingInt)) {
		if (date0->Year < date1->Year) return (-1);
		if (date0->Year > date1->Year) return  (1);
	}
	if (date0->Month == MFDefaultMissingInt) return (date1->Month == MFDefaultMissingInt ? 0 : -1);
	else {
		if (date1->Month == MFDefaultMissingInt) return (0);
		if (date0->Month < date1->Month) return (-1);
		if (date0->Month > date1->Month) return  (1);
	}
	if (date0->Day   == MFDefaultMissingInt) return (date1->Day   == MFDefaultMissingInt ? 0 : -1);
	else {
		if (date1->Day   == MFDefaultMissingInt) return (0);
		if (date0->Day   < date1->Day)   return (-1);
		if (date0->Day   > date1->Day)   return  (1);
	}
	return (0);
}

char *MFDateGetNext () {
	static char time [MFDateStringLength];
	static struct MFDate_s date;
	int month;

	date.Year  = _MFCurrentDate.Year;
	date.Month = _MFCurrentDate.Month;
	date.Day   = _MFCurrentDate.Day + 1;

	if (date.Day > _MFDateMonthLength (date.Year, date.Month)) {
		date.Day = 1;
		date.Month++;
		if (date.Month > 12) {
			date.Month = 1;
			if (date.Year != MFDefaultMissingInt) date.Year++;
		}
	}

	date.DayOfYear = 0;
	for (month = 1;month < _MFCurrentDate.Month;++month)
		date.DayOfYear += _MFDateMonthLength (date.Year,month);
	date.DayOfYear += date.Day;

	if (date.Year != MFDefaultMissingInt) sprintf (time,"%4d",date.Year);
	else strcpy (time,MFDateClimatologyYearStr);

	if (date.Month   == MFDefaultMissingInt) return (time);
	sprintf (time + strlen (time),"-%02d",date.Month);
	if (date.Day     == MFDefaultMissingInt) return (time);
	sprintf (time + strlen (time),"-%02d",date.Day);
	return (time);
}

bool MFDateCompare (const char *time0,const char *time1) {
	int pos, len;
	pos = (strncmp (time0,MFDateClimatologyYearStr,strlen (MFDateClimatologyYearStr)) == 0) ||
          (strncmp (time1,MFDateClimatologyYearStr,strlen (MFDateClimatologyYearStr)) == 0) ? 4 : 0;
	len = strlen (time0 + pos) < strlen (time1 + pos) ? strlen (time0 + pos) : strlen (time1 + pos);
	if ((pos > 0) && (strcmp (time0 + pos, "-02-29") == 0) && (strcmp (time1 + pos, "-02-28") == 0)) return (true);
	return (strncmp (time0 + pos,time1 + pos,len) == 0 ? true : false);
}

int  MFDateTimeStepLength (const char *timeStr, int timeStep) {
    int year, month, leapYear, monthLen;
    int nStep = 0;
    char scanStr [strlen (MFDateClimatologyYearStr) + 1];

    switch (timeStep) {
        case MFTimeStepYear:  if (strlen (timeStr) ==  4) return (1); break;
        case MFTimeStepMonth: if (strlen (timeStr) ==  7) return (1); break;
        case MFTimeStepDay:   if (strlen (timeStr) == 10) return (1); break;
        case MFTimeStepHour:  if (strlen (timeStr) == 13) return (1); break;
    }
    if (strncmp (timeStr,MFDateClimatologyYearStr,strlen (MFDateClimatologyYearStr)) == 0) {
        year = MFDefaultMissingInt;
        leapYear = 0;
    }
    else {
        strncpy (scanStr,timeStr,strlen (MFDateClimatologyYearStr));
        if (sscanf (scanStr,"%d", &year) != 1) {
            CMmsgPrint (CMmsgUsrError,"Date scanning error in: %s:%d",__FILE__,__LINE__);
            return (0);
        }
        leapYear = _MFDateLeapYear (year);
    }
    if (strlen (timeStr) > strlen (MFDateClimatologyYearStr)) {
        strncpy (scanStr,timeStr + strlen (MFDateClimatologyYearStr) + 1,2);
        if (sscanf (scanStr,"%d", &month) != 1) {
            CMmsgPrint (CMmsgUsrError,"Date scanning error in: %s:%d",__FILE__,__LINE__);
            return (0);
        }
        monthLen = _MFDateMonthLength(year,month);
    }

    switch (strlen (timeStr)) {
        case 4:
            switch (timeStep) {
                case MFTimeStepYear:  nStep  = 1; break;
                default:              nStep  = 0; break;
            }
            break;
        case 7:
            switch (timeStep) {
                case MFTimeStepYear:  nStep  = 12; break;
                case MFTimeStepMonth: nStep  =  1; break;
                default:              nStep  =  0; break;
            }
            break;
        case 10:
            switch (timeStep) {
                case MFTimeStepYear:  nStep = 365 + leapYear; break;
                case MFTimeStepMonth: nStep = monthLen;       break;
                case MFTimeStepDay:   nStep = 1;              break;
                default:              nStep = 0;              break;
            }
            break;
        case 13:
            switch (timeStep) {
                case MFTimeStepYear:  nStep = (365 + leapYear) * 24;  break;
                case MFTimeStepMonth: nStep = monthLen * 24;          break;
                case MFTimeStepDay:   nStep = 24;                     break;
                case MFTimeStepHour:  nStep = 1;                      break;
                default:              nStep = 0;                      break;
            }
            break;
    }
    return (nStep);
}