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

#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QString>

class QWidget;
class ProjectFile;

/// @addtogroup GUI
/// @{

/**
* @brief A class that contains project data and manages projects.
* Currently only project file creation and editing is implemented.
*/
class Project : public QObject
{
    Q_OBJECT

public:
    Project(QWidget *parent = 0);
    Project(const QString &filename, QWidget *parent = 0);
    ~Project();

    /**
    * @brief Set filename for the project file.
    * @param filename Filename.
    */
    void SetFilename(const QString &filename);

    /**
    * @brief Open existing project file.
    */
    bool Open();

    /**
    * @brief Edit the project file.
    */
    void Edit();

    /**
    * @brief Create new project file.
    */
    void Create();

    /**
    * @brief Return current project file.
    * @return project file.
    */
    ProjectFile * GetProjectFile() const
    {
        return mPFile;
    }

private:

    QString mFilename;
    ProjectFile *mPFile;
    QWidget *mParentWidget;
};

/// @}

#endif // PROJECT_H
