/*******************************************************************************
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************/

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <time.h>
#include <sys/time.h>
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include "string.h"

#include "EZMQLogger.h"

// log level
static int g_level = DEBUG;
// private log messages are not logged unless they have been explicitly enabled by calling EZMQSetLogLevel().
static bool g_hidePrivateLogEntries = true;
// Show 16 bytes, 2 chars/byte, spaces between bytes, null termination
static const uint16_t LINE_BUFFER_SIZE = (16 * 2) + 16 + 1;
static const char *LEVEL[] __attribute__ ((unused)) = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};

static bool AdjustAndVerifyLogLevel(int* level)
{
    int localLevel = *level;
    if (EZMQ_LOG_PRIVATE_DATA & localLevel)
    {
        if (g_hidePrivateLogEntries)
        {
            return false;
        }

        localLevel &= ~EZMQ_LOG_PRIVATE_DATA;
    }

    if (g_level > localLevel)
    {
        return false;
    }

    *level = localLevel;
    return true;
}

void EZMQLogBuffer(int level, const char* tag, const uint8_t* buffer, size_t bufferSize)
{
    if (!buffer || !tag || (bufferSize == 0))
    {
        return;
    }

    if (!AdjustAndVerifyLogLevel(&level))
    {
        return;
    }

    char lineBuffer[LINE_BUFFER_SIZE];
    memset(lineBuffer, 0, sizeof lineBuffer);
    size_t lineIndex = 0;
    size_t i ;
    for (i = 0; i < bufferSize; i++)
    {
        // Format the buffer data into a line
        snprintf(&lineBuffer[lineIndex * 3], sizeof(lineBuffer) - lineIndex * 3, "%02X ", buffer[i]);
        lineIndex++;
        // Output 16 values per line
        if (((i + 1) % 16) == 0)
        {
            EZMQLogv(level, tag, "%s", lineBuffer);
            memset(lineBuffer, 0, sizeof lineBuffer);
            lineIndex = 0;
        }
    }
    // Output last values in the line, if any
    if (bufferSize % 16)
    {
        EZMQLogv(level, tag, "%s", lineBuffer);
    }
}

void EZMQSetLogLevel(LogLevel level, bool hidePrivateLogEntries)
{
    g_level = level;
    g_hidePrivateLogEntries = hidePrivateLogEntries;
}
void EZMQLogv(int level, const char * tag, const char * format, ...)
{
    if (!format || !tag)
    {
        return;
    }

    if (!AdjustAndVerifyLogLevel(&level))
    {
        return;
    }

    char buffer[MAX_LOG_V_BUFFER_SIZE] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof buffer - 1, format, args);
    va_end(args);
    EZMQLog(level, tag, buffer);
}
void EZMQLog(int level, const char * tag, const char * logStr)
{
    if (!logStr || !tag)
    {
       return;
    }

    if (!AdjustAndVerifyLogLevel(&level))
    {
        return;
    }

   int min = 0;
   int sec = 0;
   int ms = 0;
#if defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0
   struct timespec when = { .tv_sec = 0, .tv_nsec = 0 };
   clockid_t clk = CLOCK_REALTIME;
#ifdef CLOCK_REALTIME_COARSE
   clk = CLOCK_REALTIME_COARSE;
#endif
   if (!clock_gettime(clk, &when))
   {
       min = (when.tv_sec / 60) % 60;
       sec = when.tv_sec % 60;
       ms = when.tv_nsec / 1000000;
   }
#elif defined(_WIN32)
   SYSTEMTIME systemTime = {0};
   GetLocalTime(&systemTime);
   min = (int)systemTime.wMinute;
   sec = (int)systemTime.wSecond;
   ms  = (int)systemTime.wMilliseconds;
#else
    struct timeval now;
    if (!gettimeofday(&now, NULL))
    {
        min = (now.tv_sec / 60) % 60;
        sec = now.tv_sec % 60;
        ms = now.tv_usec * 1000;
    }
#endif
    printf("%02d:%02d.%03d %s: %s: %s\n", min, sec, ms, LEVEL[level], tag, logStr);
}

