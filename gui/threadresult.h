/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2011 Daniel Marjamäki and Cppcheck team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef THREADRESULT_H
#define THREADRESULT_H

#include <QMutex>
#include <QObject>
#include <QStringList>
#include <QVariant>
#include "../lib/errorlogger.h"

class ErrorItem;

/// @addtogroup GUI
/// @{

/**
* @brief Threads use this class to obtain new files to process and to publish results
*
*/
class ThreadResult : public QObject, public ErrorLogger
{
    Q_OBJECT
public:
    ThreadResult();
    virtual ~ThreadResult();

    /**
    * @brief Get next unprocessed file
    * @return File path
    */
    QString GetNextFile();

    /**
    * @brief Set list of files to check
    * @param files List of files to check
    */
    void SetFiles(const QStringList &files);

    /**
    * @brief Clear files to check
    *
    */
    void ClearFiles();

    /**
    * @brief Get the number of files to check
    *
    */
    int GetFileCount();

    /**
    * ErrorLogger methods
    */
    void reportOut(const std::string &outmsg);
    void reportErr(const ErrorLogger::ErrorMessage &msg);

public slots:

    /**
    * @brief Slot threads use to signal this class that a specific file is checked
    * @param file File that is checked
    */
    void FileChecked(const QString &file);
signals:
    /**
    * @brief Progress signal
    * @param value Current progress
    */
    void Progress(int value);

    /**
    * @brief Signal of a new error
    *
    * @param item Error data
    */
    void Error(const ErrorItem &item);

    /**
    * @brief Signal of a new log message
    *
    * @param logline Log line
    */
    void Log(const QString &logline);

    /**
    * @brief Signal of a debug error
    *
    * @param item Error data
    */
    void DebugError(const ErrorItem &item);

protected:

    /**
    * @brief Mutex
    *
    */
    mutable QMutex mutex;

    /**
    * @brief List of files to check
    *
    */
    QStringList mFiles;

    /**
    * @brief Max progress
    *
    */
    int mMaxProgress;

    /**
    * @brief Current progress
    *
    */
    int mProgress;
private:
};
/// @}
#endif // THREADRESULT_H
