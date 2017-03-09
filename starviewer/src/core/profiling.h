/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

// If debug is enabled, profiling is enabled by default
#ifndef PROFILING_H
#define PROFILING_H

#ifndef QT_NO_DEBUG
    #define PROFILE_ENABLED
#endif

#ifdef PROFILE_ENABLED

#include <QString>
#include <QTime>
#include <QList>
#include <QMap>
#include <QHash>

class QIODevice;

// Warning: thread unsafe code.
namespace udg { namespace profiling {

class Profiler;
class Printer;

class Task {
friend class Profiler;
public:
    // This class is not copyable
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(const Task&&) = delete;
    Task& operator=(const Task&&) = delete;
    
    const QString& getName() const;
    int getInstance() const;
    const QTime& getStart() const;
    const QTime& getFinish() const;
    bool isFinished() const;
    int elapsed() const;
    void finish();
    
private:
    Task(const QString &name, int instance, Profiler* creator);
    
    //TODO: task starts on a member call, not on creation, to increase precision
    
    QString m_name;
    int m_instance;
    QTime m_start;
    QTime m_finish;
    
    Profiler* m_creator;
};

class Profiler {
friend class Task;
public:
    explicit Profiler(Printer* printer = 0);
    ~Profiler();
    Task& startTask(const QString &name);
    
    Printer* getPrinter();
    void setPrinter(Printer* printer);
private:
    void onTaskFinished(Task &task);

    QList<Task*> tasks;
    // Used to control the instance number for tasks with the same name
    QHash<QString, int> instanceCounter;
    Printer* m_printer;

};

class Printer
{
public:
    virtual ~Printer() = 0;
    virtual void printBegin(const Profiler& profiler) = 0;
    virtual void taskStarted(const Profiler& profiler, const Task& task) = 0;
    virtual void taskFinished(const Profiler& profiler, const Task& task) = 0;
    virtual void taskReport(const Profiler& profiler) = 0;
    virtual void printEnd(const Profiler& profiler) = 0;
};

class TextPrinter : public Printer {
public:
    explicit TextPrinter(QIODevice* output, bool showFinished = true);
    ~TextPrinter() override;
    void printBegin(const Profiler & profiler) override;
    void taskStarted(const Profiler & profiler, const Task & task) override;
    void taskFinished(const Profiler & profiler, const Task & task) override;
    void taskReport(const Profiler & profiler) override;
    void printEnd(const Profiler & profiler) override;
protected:
    void writeLine(const QString& line);
    void writeTaskLine(const Task & task, bool starting = false, bool finishing = false);
private:
    QIODevice* m_output; 
    QList<const Task*> m_tasksToReport;
    const Task* m_lastStartedTask = 0;
    const Task* m_lastFinishedTask = 0;
    bool m_showFinished;
};

class Profilers {
public:
    static void add(Profiler* profiler, const QString& name = "default");
    static bool remove(Profiler* profiler);
    static Profiler* get(const QString &name = "default");
private:
    Profilers();
    static Profilers& singleton();
    QHash<QString, Profiler*> m_profilers;
};

class FormattingHelper {
public:
    static QString toMillisecondsString(uint64_t ms);
    static QString toSecondsString(uint64_t ms);
    static QString toTimeString(uint64_t ms, bool fixedSize = false);
    
    static QString taskStatus(bool isStarting, bool isFinishing, bool isFinished, bool shortFormat = false);
    
};

}}

#endif // PROFILE_ENABLED

// **** Convenience macros to simplify task creation. ****

// NOTE: This code is an artistic expression.
// What it does is defining macros that have overloaded parameters, see how to
// use them on the class documentation.
// WARNING: Very esoteric code ahead!

#ifdef PROFILE_ENABLED
    #define PROFILING_VAR_NAME(...) PROFILING_VAR_NAME_IMPL(__VA_ARGS__, defaultTask)
    #define PROFILING_VAR_NAME_IMPL(_1, varName, ...) varName

    // Extracts the first parameter
    #define PROFILING_TASK_NAME(taskName, ...) taskName

    // Writes the profiler name if a profiler name is given
    #define PROFILING_PROFILER_NAME(...) PROFILING_PROFILER_NAME_IMPL(__VA_ARGS__, , )
    #define PROFILING_PROFILER_NAME_IMPL(_1, _2, profilerName, ...) profilerName

    // Accepts three forms of overloading:
    // PROFILE_START_TASK("taskName", variableName, "profilerName")
    // PROFILE_START_TASK("taskName", variableName) uses the "default" named profiler.
    // PROFILE_START_TASK("taskName") uses the variable name defaultTask
    
    #define PROFILE_START_TASK(...) \
    udg::profiling::Task& PROFILING_VAR_NAME(__VA_ARGS__) = udg::profiling::Profilers::get(PROFILING_PROFILER_NAME(__VA_ARGS__))->startTask(PROFILING_TASK_NAME(__VA_ARGS__)); \
    do {} while(false)
    
    #define PROFILE_FINISH_TASK(varName) \
        varName.finish(); \
        do {} while(false)
    
#else
    #define PROFILE_START_TASK(...) do {} while(false)
    #define PROFILE_FINISH_TASK(...) do {} while(false)
#endif //PROFILE_ENABLED


#endif // PROFILING_H
