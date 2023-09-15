/*
 * ExternalStatusTask.h
 *
 *  Created on: Dec 24, 2012
 *      Author: seema.sharma
 */

#ifndef EXTERNALSTATUSTASK_H_
#define EXTERNALSTATUSTASK_H_

#include "Task.h"
#include "FIFO.h"
#include "portable.h"

#define EXTSTATUS_TASK_STACKSIZE    4096
#define EXTSTATUS_TASK_FIFOSIZE     2

class ExternalStatusTask: public Task
{
public:
   ExternalStatusTask();
   Fifo<char> ExtStatusTaskQ;
   static ExternalStatusTask * thisPtr;
protected:
   virtual void Run(void);
   void Tick(void);
};
void SetAlarmStatus(BOOLEAN Status);

#endif /* EXTERNALSTATUSTASK_H_ */
