//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#ifndef __linux__
void openlog(const char *, int, int) {}
void syslog(int, const char *, ...) {}
void closelog(void) {}

int setlogmask(int) { return 0; }
#endif
