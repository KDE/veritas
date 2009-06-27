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

#ifndef VERITAS_TESTUTILS_H
#define VERITAS_TESTUTILS_H

#include <QObject>
#include <qtest_kde.h> // not used here

class QEventLoop;
class QTimer;

///////// Modified QVERIFY & QCOMPARE assertion macro's //////////////////////

#define KVERIFY(condition) QVERIFY(condition)

// TODO find out how all this new'ing & deleting by QTest::toString affects
//      test-performance

#define KVERIFY_MSG(condition,message) \
{\
    char* __failMsg__ = QTest::toString(message); \
    bool __assertion_failed__ = \
        !QTest::qVerify(condition, #condition, __failMsg__, __FILE__, __LINE__);\
    delete [] __failMsg__; \
    if (__assertion_failed__) return; \
} (void)(0)

#define KOMPARE(expected, actual) \
{ \
    char* __expected__ = QTest::toString(expected); \
    char* __actual__ = QTest::toString(actual); \
    QByteArray __failMsg__ = \
        QByteArray("\nexpected: \'").append(__expected__).append("\' actual \'").\
            append(__actual__).append("\'"); \
    delete [] __expected__; \
    delete [] __actual__; \
    if (!QTest::qVerify(expected == actual, "KOMPARE", __failMsg__.constData(), __FILE__, __LINE__))\
        return;\
} (void)(0)

#define KOMPARE_MSG(expected, actual, msg) \
{ \
    char* __expected__ = QTest::toString(expected); \
    char* __actual__ = QTest::toString(actual); \
    char* __fail_msg__ = QTest::toString(msg); \
    QByteArray __failMsg__ = \
        QByteArray("\nexpected: \'").append(__expected__).append("\' actual \'").\
            append(__actual__).append("\'\n").append(__fail_msg__); \
    delete [] __expected__; \
    delete [] __actual__; \
    delete [] __fail_msg__; \
    if (!QTest::qVerify(expected == actual, "KOMPARE_MSG", __failMsg__.constData(), __FILE__, __LINE__))\
        return;\
} (void)(0)

#define TDD_TODO QSKIP("Test command not implemented yet", SkipSingle)

//////// A couple of QTest::toString() trait implementations /////////////////

namespace QTest
{

template<> inline char* toString(const QFileInfo& fi)
{
    return qstrdup(fi.filePath().toLatin1().constData());
}

template<> inline char* toString(const QVariant& va)
{
    return qstrdup(va.toString().toLatin1().constData());
}

template<> inline char* toString(const QModelIndex& mi)
{
    return qstrdup(QString(QString::number(mi.row()) + " " +
                    QString::number(mi.column()) + " " + 
                    QString::number(mi.internalId())).toLatin1().constData());
}

} // QTest


#endif // VERITAS_TESTUTILS_H
