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

#include <QFile>
#include "profiling.h"


namespace udg { namespace  profiling {

// ******** ProfilingTask ********

const QString& Task::getName() const
{
    return m_name;
}

int Task::getInstance() const
{
    return m_instance;
}

const QTime& Task::getStart() const
{
    return m_start;
}

const QTime& Task::getFinish() const
{
    return m_finish;
}

bool Task::isFinished() const
{
    return !m_finish.isNull();
}

int Task::elapsed() const
{
    if (isFinished())
    {
        return getStart().msecsTo(getFinish());
    }
    else
    {
        return getStart().msecsTo(QTime::currentTime());
    }
}

void Task::finish()
{
    if (!isFinished())
    {
        m_finish = QTime::currentTime();
        if (m_creator)
        {
            m_creator->onTaskFinished(*this);
        }
    }
}

Task::Task(const QString& name, int instance, Profiler* creator)
{
    m_name = name;
    m_instance = instance;
    m_creator = creator;
    m_start = QTime::currentTime();
    m_finish = QTime();
}




// ******** Profiler ********

Profiler::Profiler(Printer* printer)
{
    m_printer = nullptr;
    setPrinter(printer);
}

Profiler::Profiler::~Profiler()
{
    setPrinter(nullptr);
    auto iterator = tasks.begin();
    while (iterator != tasks.end()) 
    {
        delete *iterator;
        iterator++;
    }
}

Printer * Profiler::getPrinter()
{
    return m_printer;
}
void Profiler::setPrinter(Printer* printer)
{
    if (m_printer) {
        m_printer->printEnd(*this);
    }
    m_printer = printer;
    if (m_printer) {
        m_printer->printBegin(*this);
    }
}


Task& Profiler::startTask(const QString& name)
{
    auto iterator = instanceCounter.find(name);
    if (iterator == instanceCounter.end() ) { // First insertion
        iterator = instanceCounter.insert(name, 0);
    }
    else {
        iterator.value()++;
    }
    const int& instanceNumber = *iterator;
    
    Task* task = new Task(name, instanceNumber, this);
    tasks.append(task);
    
    if (m_printer) {
        m_printer->taskStarted(*this, *task);
    }
    
    return *task;
}

void Profiler::onTaskFinished(Task& task)
{
    if (m_printer) {
        m_printer->taskFinished(*this, task);
    }
}

// ******** ProfilerPrinter ********

Printer::~Printer() {}

// ******** LogProfilerPrinter ********

TextPrinter::TextPrinter(QIODevice* output, bool showFinished)
{
    m_showFinished = showFinished;
    m_output = output;
}

TextPrinter::~TextPrinter()
{
    
}

void TextPrinter::printBegin(const Profiler& profiler)
{
    writeLine("Profiling started");
}
void TextPrinter::taskStarted(const Profiler& profiler, const Task& task)
{
    m_lastStartedTask = &task;
    m_lastFinishedTask = 0;
    m_tasksToReport.append(&task);
    taskReport(profiler);
}
void TextPrinter::taskFinished(const Profiler& profiler, const Task& task)
{
    m_lastStartedTask = 0;
    m_lastFinishedTask = &task;
    taskReport(profiler);
    if (!m_showFinished)
    {
        m_tasksToReport.removeOne(&task);
    }
}
void TextPrinter::taskReport(const Profiler& profiler)
{
    writeLine("[REPORT]");
    auto iterator = m_tasksToReport.begin();
    while (iterator != m_tasksToReport.end()) 
    {
        const Task* task = *iterator;
        if (task == m_lastStartedTask)
        {
            writeTaskLine(*task, true, false);
        }
        else if (task == m_lastFinishedTask) 
        {
            writeTaskLine(*task, false, true);
        }
        else
        {
            writeTaskLine(*task);
        }
        iterator++;
    }
    m_lastStartedTask = 0;
    m_lastFinishedTask = 0;
}

void TextPrinter::printEnd(const Profiler& profiler)
{
    writeLine("Profiling finished");
}

void TextPrinter::writeLine(const QString& line)
{
    m_output->write(QString(line + '\n').toLocal8Bit());
}


void TextPrinter::writeTaskLine(const udg::profiling::Task& task, bool starting, bool finishing)
{
    QString states[4] = {
        QString("......"), // Running
        QString("start "), // Starting
        QString("finish"), // Finishing
        QString("      ")  // Finished
    };
    int status = 0;
    if (starting) {
        status = 1;
    }
    else if (finishing) {
        status = 2;
    }
    else if (task.isFinished()) {
        status = 3;
    }
    
    int minutes = task.elapsed() / (60*1000);
    int seconds = task.elapsed() / 1000 - minutes * 60;
    int milliseconds = task.elapsed() % 1000;
    int millisecondsElapsed = task.elapsed();
    
    QString line = "%1ms %2m %3.%4s %5 (%6) %7";
    line = line
    .arg(millisecondsElapsed, 8, 10, QChar(' '))
    .arg(minutes, 2, 10, QChar('0'))
    .arg(seconds, 2, 10, QChar('0'))
    .arg(milliseconds, 3, 10, QChar('0'))
    .arg(states[status])
    .arg(task.getInstance(), 3, 10, QChar('0'))
    .arg(task.getName());
    
    writeLine(line);
    
}


// ******** Profilers ********
Profilers::Profilers()
{
}

void Profilers::add(Profiler* profiler, const QString& name)
{
    singleton().m_profilers.insert(name, profiler);
}
bool Profilers::remove(Profiler* profiler)
{
    QString key = singleton().m_profilers.key(profiler);
    return singleton().m_profilers.remove("default") >= 1;
}
Profiler* Profilers::get(const QString& name)
{
    return singleton().m_profilers.value(name, nullptr);
}

Profilers& Profilers::singleton()
{
    static Profilers profilers;
    return profilers;
}


}}
