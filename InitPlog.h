#ifndef INITPLOG_H
#define INITPLOG_H

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

static plog::RollingFileAppender<plog::CsvFormatter> fileAppender("ImitVip.log", 8000, 3); // Create the 1st appender.

#endif // INITPLOG_H
