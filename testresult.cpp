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

#include "veritas/testresult.h"
#include <KDebug>

using Veritas::TestResult;
using Veritas::TestState;

namespace Veritas
{
class TestResultPrivate
{
public:
    TestResultPrivate(TestState state, const QString& msg, int line, const QFileInfo& file) :
        m_state(state), m_message(msg), m_line(line), m_file(file) {}
    TestState m_state;
    QString m_message;
    int m_line;
    QFileInfo m_file;
};
}

using Veritas::TestResultPrivate;

TestResult::TestResult(TestState state, const QString& message, int line, const QFileInfo& file)
        : d(new TestResultPrivate(state, message, line, file))
{}

TestResult::~TestResult()
{
    delete d;
}

TestState TestResult::state() const
{
    return d->m_state;
}

QString TestResult::message() const
{
    return d->m_message;
}

int TestResult::line() const
{
    return d->m_line;
}

QFileInfo TestResult::file() const
{
    return d->m_file;
}

void TestResult::setState(TestState state)
{
    d->m_state = state;
}

void TestResult::setMessage(QString message)
{
    d->m_message = message;
}

void TestResult::setLine(int line)
{
    d->m_line = line;
}

void TestResult::setFile(QFileInfo file)
{
    d->m_file = file;
}

void TestResult::clear()
{
    d->m_state = Veritas::NoResult;
    d->m_message = "";
    d->m_line = 0;
    d->m_file = QFileInfo();
}

/*TestResult& TestResult::operator=(const TestResult& that)
{
    if (*this != that) {
        d->m_state = that.d->m_state;
        d->m_file = that.d->m_file;
        d->m_line = that.d->m_line;
        d->m_message = that.d->m_message;
    }
    return *this;
}
*/

bool TestResult::operator==(const TestResult& other) const
{
    return (d->m_state == other.d->m_state) &&
           (d->m_file == other.d->m_file) &&
           (d->m_line == other.d->m_line) &&
           (d->m_message == other.d->m_message);
}

// bool TestResult::operator!=(const TestResult& other)
// {
//     return !(*this==other);
// }

void TestResult::log() const
{
    QString result = "default";
    switch (d->m_state) {
    case Veritas::NoResult:
        result = "not set";
        break;
    case Veritas::RunSuccess:
        result = "success";
        break;
    case Veritas::RunError:
        result = "failed";
        break;
    }
    kDebug() << result << " " << d->m_message << " " << d->m_file.filePath() << " " << d->m_line;
}

void TestResult::addOutputLine(const QByteArray& line)
{
    m_output.append(line);
}

int TestResult::outputLineCount() const
{
    return m_output.count();
}

QByteArray TestResult::outputLine(int i) const
{
    return m_output.value(i);
}
