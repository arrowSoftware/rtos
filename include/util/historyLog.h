#ifndef __HISTORY_LOG_H__
#define __HISTORY_LOG_H__

void historyLog(void *argFunction, const char *argFormat, ...);
void historyLogStr(void *argFunction, const char *argFunctionName, const char *argFormat, ...);

#endif /* __HISTORY_LOG_H__ */
