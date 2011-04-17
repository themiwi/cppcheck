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

#include <QStringList>
#include <QFileInfo>
#include <QObject>
#include <QSettings>
#include <QStringList>
#include <stdlib.h>
#include "common.h"
#include "applicationlist.h"
#include "application.h"

/** name of internal editor */
static const char INTERNAL_EDITOR[] = "internal editor";

ApplicationList::ApplicationList(QObject *parent) :
    QObject(parent),
    mDefaultApplicationIndex(-1)
{
    //ctor
}

ApplicationList::~ApplicationList()
{
    Clear();
}

bool ApplicationList::LoadSettings(QSettings *programSettings)
{

    QStringList names = programSettings->value(SETTINGS_APPLICATION_NAMES, QStringList()).toStringList();
    QStringList paths = programSettings->value(SETTINGS_APPLICATION_PATHS, QStringList()).toStringList();
    QStringList params = programSettings->value(SETTINGS_APPLICATION_PARAMS, QStringList()).toStringList();
    int defapp = programSettings->value(SETTINGS_APPLICATION_DEFAULT, -1).toInt();

    // Params will be empty first time starting with the new setting.
    // Return false and inform user about problem with application settings.
    bool succeeded = true;
    if (!names.empty() && !paths.empty() && params.empty())
    {
        for (int i = 0; i < paths.length(); i++)
            params << "";
        succeeded = false;
    }

    if (names.empty() && paths.empty() && params.empty())
    {
        do
        {
            // use as default for gnome environments
            if (QFileInfo("/usr/bin/gedit").isExecutable())
            {
                Application app;
                app.setName("gedit");
                app.setPath("/usr/bin/gedit");
                app.setParameters("+(line) (file)");
                AddApplication(app);
                defapp = 0;
                break;
            }
            // use as default for kde environments
            if (QFileInfo("/usr/bin/kate").isExecutable())
            {
                Application app;
                app.setName("kate");
                app.setPath("/usr/bin/kate");
                app.setParameters("-l(line) (file)");
                AddApplication(app);
                defapp = 0;
                break;
            }
            if (FindDefaultWindowsEditor())
            {
                defapp = 0;
                break;
            }
        }
        while (0);
    }

    // Make sure there is an "internal editor" entry
    if (!names.contains(INTERNAL_EDITOR))
    {
        Application app;
        app.setName(INTERNAL_EDITOR);
        app.setPath("");
        app.setParameters("");
        AddApplication(app);
        if (names.size() == 1)
            defapp = 0;
    }

    if (names.size() > 0 && (names.size() == paths.size()))
    {
        for (int i = 0; i < names.size(); i++)
        {
            const Application app(names[i], paths[i], params[i]);
            AddApplication(app);
        }

        if (defapp == -1)
            mDefaultApplicationIndex = 0;
        else if (defapp < names.size())
            mDefaultApplicationIndex = defapp;
        else
            mDefaultApplicationIndex = 0;
    }
    return succeeded;
}

void ApplicationList::SaveSettings(QSettings *programSettings)
{
    QStringList names;
    QStringList paths;
    QStringList params;

    for (int i = 0; i < GetApplicationCount(); i++)
    {
        Application app = GetApplication(i);
        names << app.getName();
        paths << app.getPath();
        params << app.getParameters();
    }

    programSettings->setValue(SETTINGS_APPLICATION_NAMES, names);
    programSettings->setValue(SETTINGS_APPLICATION_PATHS, paths);
    programSettings->setValue(SETTINGS_APPLICATION_PARAMS, params);
    programSettings->setValue(SETTINGS_APPLICATION_DEFAULT, mDefaultApplicationIndex);

}

int ApplicationList::GetApplicationCount() const
{
    return mApplications.size();
}

Application ApplicationList::GetApplication(const int index) const
{
    if (index >= 0 && index < mApplications.size())
    {
        return mApplications[index];
    }

    return Application(QString(), QString(), QString());
}

void ApplicationList::SetApplication(int index, const Application &app)
{
    if (index >= 0 && index < mApplications.size())
    {
        mApplications.replace(index, app);
    }
}

void ApplicationList::AddApplication(const Application &app)
{
    if (app.getName().isEmpty() || (app.getName() != INTERNAL_EDITOR && app.getPath().isEmpty()))
    {
        return;
    }
    mApplications << app;
}

void ApplicationList::RemoveApplication(const int index)
{
    mApplications.removeAt(index);
}

void ApplicationList::SetDefault(const int index)
{
    if (index < mApplications.size() && index >= 0)
    {
        mDefaultApplicationIndex = index;
    }
}

void ApplicationList::Copy(const ApplicationList *list)
{
    if (!list)
    {
        return;
    }

    Clear();

    mDefaultApplicationIndex = list->GetDefaultApplication();

    for (int i = 0; i < list->GetApplicationCount(); i++)
    {
        const Application &app = list->GetApplication(i);
        if (app.getName() != INTERNAL_EDITOR)
            AddApplication(app);

        // move default application index up one step becauce the "internal editor" is skipped
        else if (mDefaultApplicationIndex > i)
            mDefaultApplicationIndex--;
    }

    // Add internal editor to the end:
    Application internalEditor;
    internalEditor.setName(INTERNAL_EDITOR);
    AddApplication(internalEditor);
}

void ApplicationList::Clear()
{
    mApplications.clear();
    mDefaultApplicationIndex = -1;
}

bool ApplicationList::FindDefaultWindowsEditor()
{
    const QString appPath(getenv("ProgramFiles"));
    const QString notepadppPath = appPath + "\\Notepad++\\notepad++.exe";
    if (QFileInfo(notepadppPath).isExecutable())
    {
        Application app;
        app.setName("Notepad++");
        app.setPath("\"" + notepadppPath + "\"");
        app.setParameters("-n(line) (file)");
        AddApplication(app);
        return true;
    }

    const QString windowsPath(getenv("windir"));
    const QString notepadPath = windowsPath + "\\system32\\notepad.exe";
    if (QFileInfo(notepadPath).isExecutable())
    {
        Application app;
        app.setName("Notepad");
        app.setPath(notepadPath);
        app.setParameters("(file)");
        AddApplication(app);
        return true;
    }
    return false;
}
