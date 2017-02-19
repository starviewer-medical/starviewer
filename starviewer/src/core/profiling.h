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
#ifndef PROFILING_H
#define PROFILING_H

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
    QMap<QString, int> instanceCounter;
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
    //LogProfilerPrinter() = delete;
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

#endif // PROFILING_H
