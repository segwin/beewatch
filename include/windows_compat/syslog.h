//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#ifndef __linux__
#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */


#define	LOG_USER	(1<<3)	/* random user-level messages */

#define	LOG_UPTO(pri)	((1 << ((pri)+1)) - 1)	/* all priorities through pri */

#define	LOG_CONS	0x02	/* log on the console if errors in sending */
#define	LOG_NDELAY	0x08	/* don't delay open */

void openlog(const char *, int, int);
void syslog(int, const char *, ...);
void closelog(void);

int setlogmask(int);
#endif
