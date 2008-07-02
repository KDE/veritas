/*
 * This file is part of KDevelop
 * Copyright 2008 Manuel Breugelmans <mbr.nxi@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <KDebug>
#include <KProcess>
#include <KLocalizedString>

namespace Veritas
{

template<typename R, typename S>
Register<R,S>::Register()
        : m_root(""),
        c_suite("suite"),
        c_case("case"),
        c_cmd("command"),
        c_name("name")
{}

template<typename R, typename S>
Register<R,S>::~Register()
{}

template<typename R, typename S>
R* Register<R,S>::rootItem()
{
    return m_rootItem;
}

template<typename R, typename S>
bool Register<R,S>::isStartElement_(const QString& elem)
{
    return isStartElement() && (name() == elem);
}

template<typename R, typename S>
bool Register<R,S>::isEndElement_(const QString& elem)
{
    return isEndElement() && (name() == elem);
}

template<typename R, typename S>
void Register<R,S>::addFromExe(const QFileInfo& exe)
{
    KProcess* proc = new KProcess;
    setExecutable(exe);
    execute(proc);
    addFromXml(proc);
}

template<typename R, typename S>
void Register<R,S>::execute(KProcess* proc)
{
    QStringList argv;
    argv << "-proto";
    proc->setProgram(m_exe.filePath(), argv);
    kDebug() << "executing " << proc->program();
    proc->setOutputChannelMode(KProcess::SeparateChannels);
    proc->start();
    proc->waitForFinished(-1);
}

template<typename R, typename S>
void Register<R,S>::addFromXml(QIODevice* dev)
{
    // Data for column headers is stored in the root item.
    QList<QVariant> rootData;
    rootData << i18n("Test Name") << i18n("Result") << i18n("Message")
            << i18n("File Name") << i18n("Line Number");
    m_rootItem = new TestRoot(rootData);

    Q_ASSERT(dev != 0);
    setDevice(dev);
    if (!device()->isOpen())
        device()->open(QIODevice::ReadOnly);

    while (!atEnd()) {
        readNext();
        if (isStartElement_(c_suite))
            processSuite();
    }
    kError(hasError()) << errorString() << " @ " << lineNumber() << ":" << columnNumber();
}

template<typename R, typename S>
void Register<R,S>::processSuite()
{
    TestSuite* suite = new TestSuite(fetchName(), m_exe, m_rootItem);
    m_rootItem->addChild(suite);
    kDebug() << suite->name();

    while (!atEnd() && !isEndElement_(c_suite)) {
        readNext();
        if (isStartElement_(c_case))
            processCase(suite);
    }
}

template<typename R, typename S>
void Register<R,S>::processCase(TestSuite* suite)
{
    TestCase* caze = new TestCase(fetchName(), suite);
    suite->addChild(caze);
    kDebug() << caze->name();
    while (!atEnd() && !isEndElement_(c_case)) {
        readNext();
        if (isStartElement_(c_cmd))
            processCmd(caze);
    }
}

template<typename R, typename S>
void Register<R,S>::setExecutable(const QFileInfo& exe)
{
    m_exe = exe;
}

template<typename R, typename S>
void Register<R,S>::processCmd(TestCase* caze)
{
    TestCommand* cmd = new TestCommand(fetchName(), caze);
    caze->addChild(cmd);
    kDebug() << cmd->name();
}

template<typename R, typename S>
QString Register<R,S>::fetchName()
{
    return attributes().value(c_name).toString();
}

} // namespace Veritas
