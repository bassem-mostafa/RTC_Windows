// #############################################################################
// #### Copyright ##############################################################
// #############################################################################

/*
 * Copyright 2024 BaSSeM
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

// #############################################################################
// #### Description ############################################################
// #############################################################################

// #############################################################################
// #### Control Include(s) #####################################################
// #############################################################################

#include "Platform.h"

// #############################################################################
// #### Control Macro(s) #######################################################
// #############################################################################

#ifndef DEBUG
    #define DEBUG
#endif

#ifdef DEBUG
    #undef DEBUG
#endif

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

#ifdef RTC_WINDOWS

// #############################################################################
// #### Include(s) #############################################################
// #############################################################################

    #include "../../RTC_Internal.h"
    #include "RTC_Windows_Port.h"

    #include <stddef.h>
    #include <sys/time.h>
    #include <time.h>
    #include <windows.h>

// #############################################################################
// #### Private Macro(s) #######################################################
// #############################################################################

// #############################################################################
// #### Private Type(s) ########################################################
// #############################################################################

typedef struct RTC_InstanceContext
{
    time_t time;
    struct timeval timeval;
    RTC_Timestamp_t Timestamp;
} RTC_InstanceContext_t;

// #############################################################################
// #### Private Method(s) Prototype ############################################
// #############################################################################

static RTC_Year_t RTC_ToYear( int year );
static RTC_Month_t RTC_ToMonth( int month );
static RTC_Day_t RTC_ToDay( int day );
static RTC_Hour_t RTC_ToHour( int hour );
static RTC_Minute_t RTC_ToMinute( int minute );
static RTC_Second_t RTC_ToSecond( int second );
static RTC_Millisecond_t RTC_ToMillisecond( int millisecond );
static RTC_Microsecond_t RTC_ToMicrosecond( int microsecond );
static RTC_Weekday_t RTC_ToWeekday( int weekday );

static RTC_Status_t RTC_Windows_Local( RTC_Instance_t * RTC_Instance );
static RTC_Status_t RTC_Windows_UTC( RTC_Instance_t * RTC_Instance );

// #############################################################################
// #### Private Variable(s) ####################################################
// #############################################################################

// static const int RTC_Windows_Year_Offset = 1900; // Not Used
static const int RTC_Windows_Year_Min = 100;
static const int RTC_Windows_Year_Max = 199;

static const int RTC_Windows_Month_Min = 0;
static const int RTC_Windows_Month_Max = 11;

static const int RTC_Windows_Day_Min = 1;
static const int RTC_Windows_Day_Max = 31;

static const int RTC_Windows_Hour_Min = 0;
static const int RTC_Windows_Hour_Max = 23;

static const int RTC_Windows_Minute_Min = 0;
static const int RTC_Windows_Minute_Max = 59;

static const int RTC_Windows_Second_Min = 0;
static const int RTC_Windows_Second_Max = 59;

static const int RTC_Windows_Millisecond_Min = 0;
static const int RTC_Windows_Millisecond_Max = 999;

static const int RTC_Windows_Microsecond_Min = 0;
static const int RTC_Windows_Microsecond_Max = 999;

static const int RTC_Windows_Weekday_Min = 0;
static const int RTC_Windows_Weekday_Max = 6;

// #############################################################################
// #### Private Method(s) ######################################################
// #############################################################################

static RTC_Year_t RTC_ToYear( int year )
{
    RTC_Year_t RTC_Year = RTC_Year_Unknown;
    if ( RTC_Windows_Year_Min <= year && year <= RTC_Windows_Year_Max )
    {
        RTC_Year = year - RTC_Windows_Year_Min + RTC_Year_2000;
    }
    return RTC_Year;
}

static RTC_Month_t RTC_ToMonth( int month )
{
    RTC_Month_t RTC_Month = RTC_Month_Unknown;
    if ( RTC_Windows_Month_Min <= month && month <= RTC_Windows_Month_Max )
    {
        RTC_Month = month - RTC_Windows_Month_Min + RTC_Month_January;
    }
    return RTC_Month;
}

static RTC_Day_t RTC_ToDay( int day )
{
    RTC_Day_t RTC_Day = RTC_Day_Unknown;
    if ( RTC_Windows_Day_Min <= day && day <= RTC_Windows_Day_Max )
    {
        RTC_Day = day - RTC_Windows_Day_Min + RTC_Day_1;
    }
    return RTC_Day;
}

static RTC_Hour_t RTC_ToHour( int hour )
{
    RTC_Hour_t RTC_Hour = RTC_Hour_Unknown;
    if ( RTC_Windows_Hour_Min <= hour && hour <= RTC_Windows_Hour_Max )
    {
        RTC_Hour = hour - RTC_Windows_Hour_Min + RTC_Hour_00;
    }
    return RTC_Hour;
}

static RTC_Minute_t RTC_ToMinute( int minute )
{
    RTC_Minute_t RTC_Minute = RTC_Minute_Unknown;
    if ( RTC_Windows_Minute_Min <= minute && minute <= RTC_Windows_Minute_Max )
    {
        RTC_Minute = minute - RTC_Windows_Minute_Min + RTC_Minute_00;
    }
    return RTC_Minute;
}

static RTC_Second_t RTC_ToSecond( int second )
{
    RTC_Second_t RTC_Second = RTC_Second_Unknown;
    if ( RTC_Windows_Second_Min <= second && second <= RTC_Windows_Second_Max )
    {
        RTC_Second = second - RTC_Windows_Second_Min + RTC_Second_00;
    }
    return RTC_Second;
}

static RTC_Millisecond_t RTC_ToMillisecond( int millisecond )
{
    RTC_Millisecond_t RTC_Millisecond = RTC_Millisecond_Unknown;
    if ( RTC_Windows_Millisecond_Min <= millisecond && millisecond <= RTC_Windows_Millisecond_Max )
    {
        RTC_Millisecond = millisecond - RTC_Windows_Millisecond_Min + RTC_Millisecond_000;
    }
    return RTC_Millisecond;
}

static RTC_Microsecond_t RTC_ToMicrosecond( int microsecond )
{
    RTC_Microsecond_t RTC_Microsecond = RTC_Microsecond_Unknown;
    if ( RTC_Windows_Microsecond_Min <= microsecond && microsecond <= RTC_Windows_Microsecond_Max )
    {
        RTC_Microsecond = microsecond - RTC_Windows_Microsecond_Min + RTC_Microsecond_000;
    }
    return RTC_Microsecond;
}

static RTC_Weekday_t RTC_ToWeekday( int weekday )
{
    RTC_Weekday_t RTC_Weekday = RTC_Weekday_Unknown;
    if ( RTC_Windows_Weekday_Min <= weekday && weekday <= RTC_Windows_Weekday_Max )
    {
        RTC_Weekday = weekday - RTC_Windows_Weekday_Min + RTC_Weekday_Sunday;
        if ( RTC_Weekday > RTC_Weekday_Friday )
        {
            RTC_Weekday -= RTC_Weekday_Friday;
        }
    }
    return RTC_Weekday;
}

static RTC_Status_t RTC_Windows_Local( RTC_Instance_t * RTC_Instance )
{
    RTC_Status_t RTC_Status = RTC_Status_Error;
    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, RTC_Instance );
        if ( ( RTC_Status = RTC_Instance_IsValid( RTC_Instance ) ) != RTC_Status_Success )
        {
            break;
        }
        struct tm * timestamp = localtime( &RTC_Instance->Context->time );
        if ( timestamp == NULL )
        {
            RTC_Status = RTC_Status_Error;
            break;
        }

        RTC_Instance->Context->Timestamp.Year = RTC_ToYear( timestamp->tm_year );
        RTC_Instance->Context->Timestamp.Month = RTC_ToMonth( timestamp->tm_mon );
        RTC_Instance->Context->Timestamp.Day = RTC_ToDay( timestamp->tm_mday );
        RTC_Instance->Context->Timestamp.Hour = RTC_ToHour( timestamp->tm_hour );
        RTC_Instance->Context->Timestamp.Minute = RTC_ToMinute( timestamp->tm_min );
        RTC_Instance->Context->Timestamp.Second = RTC_ToSecond( timestamp->tm_sec );
        RTC_Instance->Context->Timestamp.Millisecond = RTC_ToMillisecond( UTIL_MicrosecondToMillisecond( RTC_Instance->Context->timeval.tv_usec ) );
        RTC_Instance->Context->Timestamp.Microsecond = RTC_ToMicrosecond( UTIL_Modulus( RTC_Instance->Context->timeval.tv_usec, 1000 ) );
        RTC_Instance->Context->Timestamp.Weekday = RTC_ToWeekday( timestamp->tm_wday );
        RTC_Status = RTC_Status_Success;
    }
    while ( 0 );
    return RTC_Status;
}

static RTC_Status_t RTC_Windows_UTC( RTC_Instance_t * RTC_Instance )
{
    RTC_Status_t RTC_Status = RTC_Status_Error;
    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, RTC_Instance );
        if ( ( RTC_Status = RTC_Instance_IsValid( RTC_Instance ) ) != RTC_Status_Success )
        {
            break;
        }
        struct tm * timestamp = gmtime( &RTC_Instance->Context->time );
        if ( timestamp == NULL )
        {
            RTC_Status = RTC_Status_Error;
            break;
        }

        RTC_Instance->Context->Timestamp.Year = RTC_ToYear( timestamp->tm_year );
        RTC_Instance->Context->Timestamp.Month = RTC_ToMonth( timestamp->tm_mon );
        RTC_Instance->Context->Timestamp.Day = RTC_ToDay( timestamp->tm_mday );
        RTC_Instance->Context->Timestamp.Hour = RTC_ToHour( timestamp->tm_hour );
        RTC_Instance->Context->Timestamp.Minute = RTC_ToMinute( timestamp->tm_min );
        RTC_Instance->Context->Timestamp.Second = RTC_ToSecond( timestamp->tm_sec );
        RTC_Instance->Context->Timestamp.Millisecond = RTC_ToMillisecond( UTIL_MicrosecondToMillisecond( RTC_Instance->Context->timeval.tv_usec ) );
        RTC_Instance->Context->Timestamp.Microsecond = RTC_ToMicrosecond( UTIL_Modulus( RTC_Instance->Context->timeval.tv_usec, 1000 ) );
        RTC_Instance->Context->Timestamp.Weekday = RTC_ToWeekday( timestamp->tm_wday );
        RTC_Status = RTC_Status_Success;
    }
    while ( 0 );
    return RTC_Status;
}

// #############################################################################
// #### Public Method(s) #######################################################
// #############################################################################

RTC_Status_t RTC_IsValid( RTC_t RTC )
{
    RTC_Status_t RTC_Status = RTC_Status_Error;
    do
    {
        RTC_Trace( "%s( RTC=RTC_%d )", __FUNCTION__, RTC );
        switch ( RTC )
        {
            case RTC_1:
            case RTC_2:
                RTC_Status = RTC_Status_Success;
                break;
            default:
                RTC_Status = RTC_Status_Error;
                break;
        }
    }
    while ( 0 );
    return RTC_Status;
}

RTC_Status_t RTC_Instance_Initialize( RTC_Instance_t * RTC_Instance )
{
    RTC_Status_t RTC_Status = RTC_Status_Error;
    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, RTC_Instance );
        if ( ( RTC_Status = RTC_Instance_IsValid( RTC_Instance ) ) != RTC_Status_Success )
        {
            break;
        }
        if ( RTC_Instance->Context == NULL )
        {
            RAM_Status_t RAM_Status = RAM_Status_Error;
            if ( ( RAM_Status = RAM_Allocate( RAM_1, ( RAM_Reference_t * ) &RTC_Instance->Context, UTIL_SizeOf( RTC_InstanceContext_t ) ) ) != RAM_Status_Success )
            {
                RTC_Status = RTC_Status_Error;
                break;
            }
        }
        RTC_Instance->Context->Timestamp.Year = RTC_Year_Unknown;
        RTC_Instance->Context->Timestamp.Month = RTC_Month_Unknown;
        RTC_Instance->Context->Timestamp.Day = RTC_Day_Unknown;
        RTC_Instance->Context->Timestamp.Hour = RTC_Hour_Unknown;
        RTC_Instance->Context->Timestamp.Minute = RTC_Minute_Unknown;
        RTC_Instance->Context->Timestamp.Second = RTC_Second_Unknown;
        RTC_Instance->Context->Timestamp.Millisecond = RTC_Millisecond_Unknown;
        RTC_Instance->Context->Timestamp.Microsecond = RTC_Microsecond_Unknown;
        RTC_Instance->Context->Timestamp.Weekday = RTC_Weekday_Unknown;
        RTC_Status = RTC_Status_Success;
    }
    while ( 0 );
    return RTC_Status;
}

RTC_Status_t RTC_Instance_DeInitialize( RTC_Instance_t * RTC_Instance )
{
    RTC_Status_t RTC_Status = RTC_Status_Error;
    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, RTC_Instance );
        if ( ( RTC_Status = RTC_Instance_IsValid( RTC_Instance ) ) != RTC_Status_Success )
        {
            break;
        }
        if ( RTC_Instance->Context != NULL )
        {
            RAM_Status_t RAM_Status = RAM_Status_Error;
            if ( ( RAM_Status = RAM_DeAllocate( RAM_1, ( RAM_Reference_t * ) &RTC_Instance->Context ) ) != RAM_Status_Success )
            {
                RTC_Status = RTC_Status_Error;
                break;
            }
        }
        // Nothing to be done
        RTC_Status = RTC_Status_Success;
    }
    while ( 0 );
    return RTC_Status;
}

RTC_Status_t RTC_Instance_Cycle( RTC_Instance_t * RTC_Instance )
{
    RTC_Status_t RTC_Status = RTC_Status_Error;
    do
    {
        RTC_Trace( "%s( Instance=%p )", __FUNCTION__, RTC_Instance );
        if ( ( RTC_Status = RTC_Instance_IsValid( RTC_Instance ) ) != RTC_Status_Success )
        {
            break;
        }
        if ( RTC_Instance->Context == NULL )
        {
            // Nothing to be done
            RTC_Status = RTC_Status_Success;
            break;
        }
        if ( time( &RTC_Instance->Context->time ) == -1 )
        {
            RTC_Status = RTC_Status_Error;
            break;
        }
        if ( gettimeofday( &RTC_Instance->Context->timeval, NULL ) != 0 )
        {
            RTC_Status = RTC_Status_Error;
            break;
        }
        switch ( RTC_Instance->RTC )
        {
            case RTC_1:
                RTC_Status = RTC_Windows_Local( RTC_Instance );
                break;
            case RTC_2:
                RTC_Status = RTC_Windows_UTC( RTC_Instance );
                break;
            default:
                RTC_Status = RTC_Status_Error;
                break;
        }
    }
    while ( 0 );
    return RTC_Status;
}

RTC_Status_t RTC_Instance_GetTimestamp( RTC_Instance_t * RTC_Instance, RTC_Timestamp_t * RTC_Timestamp )
{
    RTC_Status_t RTC_Status = RTC_Status_Error;
    do
    {
        RTC_Trace( "%s( Instance=%p, Timestamp=%p )", __FUNCTION__, RTC_Instance, RTC_Timestamp );
        if ( RTC_Timestamp == NULL )
        {
            RTC_Status = RTC_Status_ArgumentInvalid;
            break;
        }
        if ( ( RTC_Status = RTC_Instance_IsValid( RTC_Instance ) ) != RTC_Status_Success )
        {
            break;
        }
        *RTC_Timestamp = RTC_Instance->Context->Timestamp;
        RTC_Status = RTC_Status_Success;
    }
    while ( 0 );
    return RTC_Status;
}

RTC_Status_t RTC_Instance_SetTimestamp( RTC_Instance_t * RTC_Instance, RTC_Timestamp_t * RTC_Timestamp )
{
    RTC_Status_t RTC_Status = RTC_Status_Error;
    do
    {
        RTC_Trace( "%s( Instance=%p, Timestamp=%p )", __FUNCTION__, RTC_Instance, RTC_Timestamp );
        if ( RTC_Timestamp == NULL )
        {
            RTC_Status = RTC_Status_ArgumentInvalid;
            break;
        }
        if ( ( RTC_Status = RTC_Instance_IsValid( RTC_Instance ) ) != RTC_Status_Success )
        {
            break;
        }
        RTC_Instance->Context->Timestamp = *RTC_Timestamp;
        // FIXME Set Time
        RTC_Status = RTC_Status_NotSupported;
    }
    while ( 0 );
    return RTC_Status;
}

// #############################################################################
// #### Public Variable(s) #####################################################
// #############################################################################

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

#endif /* RTC_WINDOWS */

// #############################################################################
// #### END OF FILE ############################################################
// #############################################################################
