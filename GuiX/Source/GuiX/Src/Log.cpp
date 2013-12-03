#include <GuiX/Config.h>

#include <stdarg.h>
#include <stdio.h>

#include <GuiX/Interfaces.h>

#include <Src/Log.h>

namespace guix {
namespace core {

namespace {

static const size_t PADDING     = 13;
static const size_t LINE_W      = 48;
static const size_t BUFFER_SIZE = 512;

inline void SprintV(char* buffer, const char* fmt, va_list va)
{
	int len = vsnprintf(buffer, BUFFER_SIZE-2, fmt, va);	
	if(len < 0 || len > BUFFER_SIZE-2) len = BUFFER_SIZE-2;
	buffer[len] = 0;
}

inline void SprintF(char* buffer, const char* fmt, ...)
{
	va_list argument_list;
	va_start(argument_list, fmt);
	SprintV(buffer, fmt, argument_list);
	va_end(argument_list);
}

}; // anonymous namespace

// ===================================================================================
// GxLog
// ===================================================================================

GxLog* GxLog::singleton = NULL;

void GxLog::Create()
{
	singleton = new GxLog();
}

void GxLog::Destroy()
{
	delete singleton;
	singleton = NULL;
}

GxLog* GxLog::Get()
{
	return GxLog::singleton;
}

GxLog::~GxLog()
{
}

GxLog::GxLog()
{
	myType = GX_LT_INFO;
	myIsInBlock = false;
}

void GxLog::Write(GxLogType type, const char* component, const char* fmt, ...)
{
	Flush(myType, "");
	myType = type;

	char buffer[BUFFER_SIZE];
	va_list argument_list;
	va_start(argument_list, fmt);
	SprintV(buffer, fmt, argument_list);
	va_end(argument_list);

	Print(myType, component, buffer);
}

void GxLog::Write(const char* fmt, ...)
{
	Flush(myType, "");

	char buffer[BUFFER_SIZE];
	va_list argument_list;
	va_start(argument_list, fmt);
	SprintV(buffer, fmt, argument_list);
	va_end(argument_list);

	Print(myType, "", buffer);
}

void GxLog::Buffer(const char* fmt, ...)
{
	char buffer[BUFFER_SIZE];
	va_list argument_list;
	va_start(argument_list, fmt);
	SprintV(buffer, fmt, argument_list);
	va_end(argument_list);

	myBuffer.Append(buffer);
}

void GxLog::Flush(GxLogType type, const char* component)
{
	myType = type;
	if(!myBuffer.Empty())
	{
		Print(myType, component, myBuffer.Raw());
		myBuffer.Clear();
	}
}

void GxLog::Flush()
{
	if(!myBuffer.Empty())
	{
		Print(myType, "", myBuffer.Raw());
		myBuffer.Clear();
	}
}

void GxLog::BlockStart(GxLogType type, const char* component)
{
	myType = type;

	char buffer[BUFFER_SIZE];
	SprintF(buffer, "%-13s:", component);
	const GxString line('-', LINE_W);

	GxSystemInterface::Get()->LogMessage(myType, buffer);
	GxSystemInterface::Get()->LogMessage(myType, line.Raw());
	myIsInBlock = true;
}

void GxLog::BlockEnd()
{
	const GxString line('-', LINE_W);
	GxSystemInterface::Get()->LogMessage(myType, line.Raw());
	myIsInBlock = false;
}

void GxLog::NewLine()
{
	GxSystemInterface::Get()->LogMessage(myType, "");
}

void GxLog::Print(GxLogType type, const char* component, const char* message)
{
	myType = type;
	if(myIsInBlock)
	{
		GxSystemInterface::Get()->LogMessage(myType, message);
	}
	else
	{
		char buffer[BUFFER_SIZE];
		SprintF(buffer, "%-13s: %s", component, message);
		GxSystemInterface::Get()->LogMessage(myType, buffer);
	}
}

}; // namespace core
}; // namespace guix