// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/stdlib/stdarg.h_v   1.0   18 Aug 2014 09:16:48   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares stdarg.h functions so that we will not have to use
// libraries provided by the compiler. Their overhead is unknown and may
// interfere with our system design. This file shall only contain definitions
// one would reasonably expect to find in standard stdarg.h.

#ifndef STDARG_H_
#define STDARG_H_

#define va_start(v,l)	__builtin_va_start(v,l)
#define va_end(v)		__builtin_va_end(v)
#define va_arg(v,l)		__builtin_va_arg(v,l)

typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;

#endif
