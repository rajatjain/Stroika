/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<time.h>

#if		qPlatform_Windows
	#include	<atlbase.h>		// For CComBSTR
#elif	qPlatform_POSIX
	//not sure needed?#include	<sys/times.h>
#endif

#include	"../Characters/Format.h"
#include	"../Debug/Assertions.h"
#include	"../Execution/Exceptions.h"
#include	"../Memory/SmallStackBuffer.h"
#include	"../Linguistics/Words.h"

#include	"DateTime.h"

using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::Execution;
using	namespace	Stroika::Foundation::Memory;
using	namespace	Stroika::Foundation::Time;

using	Debug::TraceContextBumper;

using	namespace	Time;




#if		qPlatform_Windows
namespace	{
	TimeOfDay	mkTimeOfDay_ (const SYSTEMTIME& sysTime)
		{
			WORD	hour = max (sysTime.wHour, static_cast<WORD> (0));
			hour = min (hour, static_cast<WORD> (23));
			WORD	minute = max (sysTime.wMinute, static_cast<WORD> (0));
			minute = min (minute, static_cast<WORD> (59));
			WORD	secs = max (sysTime.wSecond, static_cast<WORD> (0));
			secs = min (secs, static_cast<WORD> (59));
			return TimeOfDay ((hour * 60 + minute) * 60 + secs);
		}
}
#endif







/*
 ********************************************************************************
 *********************************** DateTime ***********************************
 ********************************************************************************
 */
namespace	{
	// Once we have all our compilers supporting constexp - this can go in header (Date.inl) - and then be shared
	// across OBJS
	const	Date	kMin_	=	Date (Date::JulianRepType (Date::kMinJulianRep));	//year eFirstYear  - January
	const	Date	kMax_	=	Date (Date::JulianRepType (UINT_MAX-1));

	const	TimeOfDay	kMinT_	=	TimeOfDay (0);
	const	TimeOfDay	kMaxT_	=	TimeOfDay (24 * 60 * 60 - 1);
}
const	DateTime	DateTime::kMin	=	DateTime (kMin_, kMinT_);
const	DateTime	DateTime::kMax	=	DateTime (kMax_, kMaxT_);

DateTime::DateTime (const wstring& rep)
	: fDate ()
	, fTimeOfDay ()
{
	if (not rep.empty ()) {
#if			qPlatform_Windows
		LCID lcid = LOCALE_USER_DEFAULT;
		DATE		d;
		(void)::memset (&d, 0, sizeof (d));
		try {
			ThrowIfErrorHRESULT (::VarDateFromStr (CComBSTR (rep.c_str ()), lcid, 0, &d));
		}
		catch (...) {
			// though COULD be time format exception?
			Execution::DoThrow (Date::FormatException ());
		}
		// SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
		SYSTEMTIME	sysTime;
		memset (&sysTime, 0, sizeof (sysTime));
		Verify (::VariantTimeToSystemTime (d, &sysTime));
		fDate = Date (sysTime);
		fTimeOfDay = mkTimeOfDay_ (sysTime);
#elif	qPlatform_POSIX
		AssertNotImplemented ();
#else
		AssertNotImplemented ();
#endif
	}
}

#if		qPlatform_Windows
DateTime::DateTime (const wstring& rep, LCID lcid)
	: fDate ()
	, fTimeOfDay ()
{
	if (not rep.empty ()) {
		DATE		d;
		(void)::memset (&d, 0, sizeof (d));
		try {
			ThrowIfErrorHRESULT (::VarDateFromStr (CComBSTR (rep.c_str ()), lcid, 0, &d));
		}
		catch (...) {
			// though COULD be time format exception?
			Execution::DoThrow (Date::FormatException ());
		}
		// SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
		SYSTEMTIME	sysTime;
		memset (&sysTime, 0, sizeof (sysTime));
		Verify (::VariantTimeToSystemTime (d, &sysTime));
		fDate = Date (sysTime);
		fTimeOfDay = mkTimeOfDay_ (sysTime);
	}
}
#endif

DateTime::DateTime (const wstring& rep, XML):
	fDate (),
	fTimeOfDay ()
{
	if (not rep.empty ()) {
		int	year	=	0;
		int	month	=	0;
		int	day		=	0;
		int	hour	=	0;
		int	minute	=	0;
		int	second	=	0;
		int	tzHr	=	0;
		int	tzMn	=	0;
		#pragma	warning (push)
		#pragma	warning (4 : 4996)		// MSVC SILLY WARNING ABOUT USING swscanf_s
		int	nItems	=	::swscanf (rep.c_str (), L"%d-%d-%dT%d:%d:%d-%d:%d", &year, &month, &day, &hour, &minute, &second, &tzHr, &tzMn);
		#pragma warning (pop)
		if (nItems >= 3) {
			fDate = Date (::Format (L"%d-%d-%d", year, month, day), Date::eXML);
		}
		if (nItems >= 5) {
			fTimeOfDay = TimeOfDay (hour * 60 * 60 + minute * 60 + second);
		}
		if (nItems >= 8) {
			// CHECK TZ
			// REALLY - must check TZ - but must adjust value if currentmachine timezone differs from one found in file...
		}
	}
}

#if		qPlatform_Windows
DateTime::DateTime (const SYSTEMTIME& sysTime):
	fDate (sysTime),
	fTimeOfDay (mkTimeOfDay_ (sysTime))
{
}

DateTime::DateTime (const FILETIME& fileTime):
	fDate (),
	fTimeOfDay ()
{
	FILETIME localTime;
	(void)::memset (&localTime, 0, sizeof (localTime));
	if (::FileTimeToLocalFileTime (&fileTime, &localTime)) {
		SYSTEMTIME sysTime;
		(void)::memset (&sysTime, 0, sizeof (sysTime));
		if (::FileTimeToSystemTime (&localTime, &sysTime)) {
			fDate= Date (sysTime);
			fTimeOfDay = mkTimeOfDay_ (sysTime);
		}
	}
}
#endif

DateTime::DateTime (time_t unixTime):
	fDate (),
	fTimeOfDay ()
{
#if		qPlatform_Windows
	// From http://support.microsoft.com/kb/167296
	FILETIME	ft;
	LONGLONG ll;
	ll = Int32x32To64 (unixTime, 10000000) + 116444736000000000;
	ft.dwLowDateTime = (DWORD)ll;
	ft.dwHighDateTime = static_cast<DWORD> (ll >> 32);
	*this = DateTime (ft);
#elif	qPlatform_POSIX
	time_t	clk = time (0);
	const tm* now = localtime (&clk);
	fDate = Date (Date::Year (now->tm_year+1900), Date::MonthOfYear (now->tm_mon+1), Date::DayOfMonth (now->tm_mday));

	fTimeOfDay = TimeOfDay (now->tm_sec + (now->tm_min * 60) + (now->tm_hour * 60 * 60));
#else
	Assert (false);
#endif
}

bool	DateTime::empty () const
{
	// Risky change so late in the game - but this logic seems wrong (and causes some trouble).
	// DateTime is NOT empty just because date part is empty. We CAN use a DateTime record to store JUST a time!
	//		-- LGP 2006-04-26
	return fDate.empty () and fTimeOfDay.empty ();
}

DateTime	DateTime::Now ()
{
#if		qPlatform_Windows
	SYSTEMTIME	st;
	memset (&st, 0, sizeof (st));
	::GetLocalTime (&st);
	return DateTime (st);
#elif	qPlatform_POSIX
	return DateTime (time (nullptr));
#else
	AssertNotImplemented ();
	return DateTime ();
#endif
}

wstring	DateTime::Format () const
{
#if		qPlatform_Windows
	return Format (LOCALE_USER_DEFAULT);
#elif	qPlatform_POSIX
	AssertNotImplemented ();
	return wstring ();
#else
	AssertNotImplemented ();
	return wstring ();
#endif
}

#if		qPlatform_Windows
wstring	DateTime::Format (LCID lcid) const
{
	if (empty ()) {
		return wstring ();
	}
	else {
		wstring	r	=	fDate.Format (lcid);
		Assert (not r.empty ());
		wstring	tod	=	fTimeOfDay.Format (lcid);
		if (not tod.empty ()) {
			r += L" " + tod;
		}
		return r;
	}
}
#endif

wstring	DateTime::Format4XML () const
{
	if (empty ()) {
		return wstring ();
	}
	else {
		wstring	r	=	fDate.Format4XML ();
		if (not fTimeOfDay.empty ()) {
			// be sure using DateWithOptionalTime

			// something like append T22:33:11 - apx...
			wchar_t	buf[1024];
			buf[0] = 0;
			unsigned int	t	=	fTimeOfDay.GetAsSecondsCount ();
			struct tm	 temp;
			memset (&temp, 0, sizeof (temp));
			temp.tm_hour = t/(60*60);
			temp.tm_min = (t - temp.tm_hour * 60 * 60) / 60;
			temp.tm_sec = (t - temp.tm_hour * 60 * 60 - temp.tm_min * 60);
			wcsftime (buf, NEltsOf (buf), L"%H:%M:%S", &temp);

			wstring	tzBiasString;
			{
#if		qPlatform_Windows
				TIME_ZONE_INFORMATION	tzInfo;
				memset (&tzInfo, 0, sizeof (tzInfo));
				(void)::GetTimeZoneInformation (&tzInfo);
				int unsignedBias	=	abs (tzInfo.Bias);
				int	hrs	=	unsignedBias / 60;
				int mins = unsignedBias - hrs * 60;
				tzBiasString = ::Format (L"%s%.2d:%.2d", (tzInfo.Bias >= 0? L"-": L"+"), hrs, mins);
#elif	qPlatform_POSIX
				//AssertNotImplemented ();
				// WRONG - but let things limp along for a little while...
				//		--:LGP 2011-09-28
#else
				AssertNotImplemented ();
#endif
			}
			r += wstring (L"T") + buf + tzBiasString;
		}
		Assert (DateTime (r, eXML) == *this);
		return r;
	}
}

namespace	{
#if		qPlatform_Windows
	SYSTEMTIME	toSysTime_ (TimeOfDay tod)
		{
			SYSTEMTIME	t;
			memset (&t, 0, sizeof (t));
			if (not tod.empty ()) {
				unsigned int	seconds	=	tod.GetAsSecondsCount ();
				unsigned int	minutes	=	seconds / 60;
				unsigned int	hours	=	minutes / 60;

				hours = min (hours, 23U);
				t.wHour = hours;

				minutes -= hours * 60;
				minutes = min (minutes, 59U);
				t.wMinute = minutes;

				seconds -= (60*60 * hours + 60 * minutes);
				seconds = min (seconds, 59U);
				t.wSecond = seconds;
			}
			return t;
		}
#endif
}

#if		qPlatform_Windows
DateTime::operator SYSTEMTIME () const
{
	SYSTEMTIME	d	=	(SYSTEMTIME)fDate;
	SYSTEMTIME	t	=	(SYSTEMTIME)toSysTime_ (fTimeOfDay);
	SYSTEMTIME	r	=	d;
	r.wHour = t.wHour;
	r.wMinute = t.wMinute;
	r.wSecond = t.wSecond;
	r.wMilliseconds = t.wMilliseconds;
	return r;
}
#endif

Date::JulianRepType	DateTime::DaysSince () const
{
	int	r	=	DayDifference (GetToday (), *this);
	if (r < 0) {
		return 0;
	}
	else {
		return r;
	}
}

#if		qPlatform_Windows
namespace	{
	time_t	OLD_GetUNIXEpochTime_ (const DateTime& dt)
		{
			SYSTEMTIME	st	=	dt;
			struct tm tm;
			memset(&tm, 0, sizeof(tm));
			tm.tm_year = st.wYear - 1900;
			tm.tm_mon = st.wMonth - 1;
			tm.tm_mday = st.wDay;
			tm.tm_hour = st.wHour;
			tm.tm_min = st.wMinute;
			tm.tm_sec = st.wSecond;
			return mktime (&tm);
		}
}
#endif

time_t	DateTime::GetUNIXEpochTime () const
{
	struct tm tm;
	memset(&tm, 0, sizeof(tm));
	tm.tm_year = fDate.GetYear () - 1900;
	tm.tm_mon = fDate.GetMonth () - 1;
	tm.tm_mday = fDate.GetDayOfMonth ();
	unsigned int	totalSecondsRemaining	=	fTimeOfDay.GetAsSecondsCount ();
	tm.tm_hour = totalSecondsRemaining / (60 * 60);
	totalSecondsRemaining -= tm.tm_hour * 60 * 60;
	tm.tm_min = totalSecondsRemaining / 60;
	totalSecondsRemaining -= tm.tm_min * 60;
	tm.tm_sec = totalSecondsRemaining;
	time_t	result	=	mktime (&tm);
	#if		qPlatform_Windows
	Ensure (result == OLD_GetUNIXEpochTime_ (*this));		// OLD WINDOZE code was WRONG - neglecting the coorect for mktime () timezone nonsense
	#endif
	/*
	 * This is PURELY to correct for the fact that mktime() uses the current timezone - and has NOTHING todo with the timezone assocaited with teh given
	 * DateTime() object.
	 */
	result += Time::GetLocaltimeToGMTOffset ();
	return result;
}

void	DateTime::SetDate (const Date& d)
{
	fDate = d;
}

void		DateTime::SetTimeOfDay (const TimeOfDay& tod)
{
	fTimeOfDay = tod;
}

bool Time::operator< (const DateTime& lhs, const DateTime& rhs)
{
	if (lhs.GetDate () < rhs.GetDate ()) {
		return true;
	}
	if (lhs.GetDate () == rhs.GetDate ()) {
		return lhs.GetTimeOfDay () < rhs.GetTimeOfDay ();
	}
	return false;
}

bool Time::operator<= (const DateTime& lhs, const DateTime& rhs)
{
	return lhs < rhs or lhs == rhs;
}

bool Time::operator> (const DateTime& lhs, const DateTime& rhs)
{
	if (lhs.GetDate () > rhs.GetDate ()) {
		return true;
	}
	if (lhs.GetDate () == rhs.GetDate ()) {
		return lhs.GetTimeOfDay () > rhs.GetTimeOfDay ();
	}
	return false;
}

bool Time::operator>= (const DateTime& lhs, const DateTime& rhs)
{
	return lhs == rhs or lhs > rhs;
}

bool Time::operator== (const DateTime& lhs, const DateTime& rhs)
{
	return lhs.GetDate () == rhs.GetDate () and lhs.GetTimeOfDay () == rhs.GetTimeOfDay ();
}

bool Time::operator!= (const DateTime& lhs, const DateTime& rhs)
{
	return lhs.GetDate () != rhs.GetDate () or lhs.GetTimeOfDay () != rhs.GetTimeOfDay ();
}


