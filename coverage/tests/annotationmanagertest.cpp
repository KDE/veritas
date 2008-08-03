/* KDevelop xUnit plugin
 *
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

#include "annotationmanagertest.h"
#include <veritas/coverage/annotationmanager.h>
#include <veritas/coverage/coveredfile.h>
#include <veritas/tests/kasserts.h>
#include <interfaces/icore.h>
#include <interfaces/idocument.h>
#include <sublime/controller.h>

#include <kparts/mainwindow.h>
#include <ktexteditor/document.h>
#include <ktexteditor/view.h>
#include <ktexteditor/annotationinterface.h>

#include "corestub.h"
#include "documentstub.h"
#include "viewstub.h"

#include <qtest_kde.h>
#include <QTextStream>

using KDevelop::ICore;
using KDevelop::IDocument;
using KDevelop::ILanguageController;
using KDevelop::IProjectController;
using KDevelop::IUiController;
using KDevelop::IRunController;

using Veritas::AnnotationManager;
using Veritas::AnnotationManagerTest;
using Veritas::CoveredFile;

using KTextEditor::AnnotationModel;
using KTextEditor::Cursor;
using KTextEditor::Range;
using KTextEditor::View;

using TestStubs::KDevDocument;
using TestStubs::Core;
using TestStubs::TextDocument;
using TestStubs::UiController;

///////////////// fixture ////////////////////////////////////////////////////

void AnnotationManagerTest::initTestCase()
{
    m_core = new TestStubs::Core;
    Q_ASSERT(m_core);
    UiController* uic = new TestStubs::UiController;
    m_core->m_uiController = uic;
    uic->m_activeMainWindow = new KParts::MainWindow;
    uic->m_controller = new Sublime::Controller;
    m_docCtrl = new TestStubs::DocumentController(0);
    m_core->m_documentController = m_docCtrl;
}

void AnnotationManagerTest::init()
{
    m_manager = new AnnotationManager;
    m_someUrl = KUrl("foo.cpp");
    m_widget = new QWidget;
    m_lineNr = 10;
    m_nrofCalls = 5;
}

void AnnotationManagerTest::cleanup()
{
    //if (m_manager) delete m_manager;
}

///////////////// commands ///////////////////////////////////////////////////

void AnnotationManagerTest::instantiate()
{
    QMap<KUrl, CoveredFile*> files;
    m_manager->setCoveredFiles(files);
    KDevDocument* doc = createKDevDocument(m_someUrl); 
    m_manager->watch(doc);
    m_manager->stopWatching(doc);
}

void AnnotationManagerTest::singleLine()
{
    // setup
    CoveredFile* f = createCoveredFile(m_lineNr, m_nrofCalls, m_someUrl);
    m_manager->addCoverageData(f);
    KDevDocument* doc = createKDevDocument(f->url());
    m_manager->watch(doc);

    View* v = triggerAnnotationsOnView(doc);                // exercise
    assertAnnoCallCountEquals(m_lineNr, m_nrofCalls, v);    // verify
}

void AnnotationManagerTest::multipleLines()
{
    // setup
    CoveredFile* f = createCoveredFile(m_lineNr, m_nrofCalls, m_someUrl);
    int line2 = 20, callCount2 = 10;
    f->setCallCount(line2, callCount2);
    m_manager->addCoverageData(f);
    KDevDocument* doc = createKDevDocument(f->url());
    m_manager->watch(doc);

    View* v = triggerAnnotationsOnView(doc);                 // exercise
    assertAnnoCallCountEquals(m_lineNr, m_nrofCalls, v);     // verify
    assertAnnoCallCountEquals(line2, callCount2, v);
}

void AnnotationManagerTest::multipleViews()
{
    // setup
    CoveredFile* f = createCoveredFile(m_lineNr, m_nrofCalls, m_someUrl);
    m_manager->addCoverageData(f);
    KDevDocument* doc = createKDevDocument(f->url());
    m_manager->watch(doc);

    // exercise
    View* v1 = triggerAnnotationsOnView(doc);
    // inject another view in the stub
    TextDocument* tdoc = (TextDocument*)doc->textDocument();
    tdoc->m_createView = new TestStubs::TextView(0);
    View* v2 = triggerAnnotationsOnView(doc);
    Q_ASSERT(v1 != v2);

    // verify
    assertAnnoCallCountEquals(m_lineNr, m_nrofCalls, v1);
    assertAnnoCallCountEquals(m_lineNr, m_nrofCalls, v2);
}

void AnnotationManagerTest::multipleFiles()
{
    // setup
    CoveredFile* f1  = createCoveredFile(m_lineNr, m_nrofCalls, m_someUrl);
    m_manager->addCoverageData(f1);
    int line2 = 20, nrofCalls2 = 5;
    CoveredFile* f2 = createCoveredFile(line2, nrofCalls2, KUrl("bar.cpp"));
    m_manager->addCoverageData(f2);

    KDevDocument* doc1 = createKDevDocument(f1->url());
    m_manager->watch(doc1);
    KDevDocument* doc2 = createKDevDocument(f2->url());
    m_manager->watch(doc2);

    // exercise
    View* v1 = triggerAnnotationsOnView(doc1);
    View* v2 = triggerAnnotationsOnView(doc2);

    // verify
    assertAnnoCallCountEquals(m_lineNr, m_nrofCalls, v1);
    assertAnnoCallCountEquals(line2, nrofCalls2, v2);
}

void AnnotationManagerTest::notCovered()
{
    // setup
    m_lineNr = 5;
    CoveredFile* f  = createCoveredFile(5, m_nrofCalls, m_someUrl);
    m_manager->addCoverageData(f);
    KDevDocument* doc = createKDevDocument(f->url());
    m_manager->watch(doc);

    // exercise
    View* v = triggerAnnotationsOnView(doc);

    // verify
    assertNoAnnotationsOnLine(4,  v);
    assertNoAnnotationsOnLine(6,  v);
    assertNoAnnotationsOnLine(10, v);
}

void AnnotationManagerTest::multiCoverageOneFile()
{
    CoveredFile* f1 = createCoveredFile(m_lineNr, m_nrofCalls, m_someUrl);
    CoveredFile* f2 = createCoveredFile(m_lineNr+1, m_nrofCalls+1, m_someUrl);
    m_manager->addCoverageData(f1);
    m_manager->addCoverageData(f2);
    KDevDocument* doc = createKDevDocument(m_someUrl);
    m_manager->watch(doc);

    View* v = triggerAnnotationsOnView(doc);
    assertAnnoCallCountEquals(m_lineNr, m_nrofCalls, v);
    assertAnnoCallCountEquals(m_lineNr+1, m_nrofCalls+1, v);
}


void AnnotationManagerTest::stopWatching()
{
    // setup
    CoveredFile* f = createCoveredFile(m_lineNr, m_nrofCalls, m_someUrl);
    m_manager->addCoverageData(f);
    KDevDocument* doc = createKDevDocument(f->url());
    m_manager->watch(doc);

    // exercise
    View* v = triggerAnnotationsOnView(doc);
    m_manager->stopWatching(doc);

    // verify
    KTextEditor::AnnotationViewInterface *anno =
            qobject_cast<KTextEditor::AnnotationViewInterface*>(v);
    Q_ASSERT_X(anno, "assertAnnoCallCountEquals()", "Err?");
    KVERIFY(not anno->isAnnotationBorderVisible());
    AnnotationModel* model = anno->annotationModel();
    KVERIFY(not model);
}

////////////////// setup helpers /////////////////////////////////////////////

void AnnotationManagerTest::initManager(CoveredFile* f)
{
    //QMap<KUrl, CoveredFile*> files;
    //files[f->url()] = f;
    //m_manager->setCoveredFiles(files);
}

KDevDocument* AnnotationManagerTest::createKDevDocument(const KUrl& u)
{
    TestStubs::KDevDocument* kdoc = new TestStubs::KDevDocument(m_core);
    kdoc->m_url = u;
    TestStubs::TextDocument* doc = new TestStubs::TextDocument;
    kdoc->m_textDocument = doc;
    doc->m_createView = new TestStubs::TextView(0);
    return kdoc;
}

CoveredFile* AnnotationManagerTest::createCoveredFile(int line, int callCount, KUrl url)
{
    CoveredFile* f = new CoveredFile();
    f->setUrl(url);
    f->setCallCount(line, callCount);
    return f;
}

/////////////////// exercise helpers /////////////////////////////////////////

KTextEditor::View* AnnotationManagerTest::triggerAnnotationsOnView(KDevelop::IDocument* doc)
{
    m_docCtrl->emitTextDocumentCreated(doc); // fake activate the doc.
    QTest::qWait(50);
    KTextEditor::Document* tdoc = doc->textDocument();
     // now spawn a view which should trigger annotations being set.
    KTextEditor::View* v = tdoc->createView(m_widget);
    QTest::qWait(50);
    return v;
}

////////////////// custom assertions /////////////////////////////////////////

AnnotationModel* annotationModelForView(View* v)
{
    KTextEditor::AnnotationViewInterface *anno =
            qobject_cast<KTextEditor::AnnotationViewInterface*>(v);
    Q_ASSERT_X(anno, "assertAnnoCallCountEquals()", "Err?");
    Q_ASSERT(anno->isAnnotationBorderVisible());
    KTextEditor::AnnotationModel* model = anno->annotationModel();
    return model;
}

void AnnotationManagerTest::assertAnnoCallCountEquals(int line, int count, KTextEditor::View* v)
{
    AnnotationModel* model = annotationModelForView(v);
    KVERIFY_MSG(model != 0, "Annotation model not set");
    QString expected = QString::number(count);
    // annotationmodel starts counting from zero
    QString actual = model->data(line-1, Qt::DisplayRole).toString();
    QString failureMsg;
    QTextStream str(&failureMsg);
    str << "Wrong call count for line " << line
        << " was expecting " << expected << " but got " << actual;
    KOMPARE_MSG(expected, actual, failureMsg);
}

void AnnotationManagerTest::assertNoAnnotationsOnLine(int line, KTextEditor::View* v)
{
    AnnotationModel* model = annotationModelForView(v);
    KVERIFY_MSG(model != 0, "Annotation model not set");
    QVariant expected; // should return default qvariant.
    QVariant actual = model->data(line-1, Qt::DisplayRole);
    QString failureMsg;
    QTextStream str(&failureMsg);
    str << "Was not expecting annotation on line "
        << line << " but got " << actual.toString();
    KOMPARE_MSG(expected, actual, failureMsg);
}

void AnnotationManagerTest::assertAnnotationsGone(View* v)
{
    AnnotationModel* model = annotationModelForView(v);
    KVERIFY_MSG(not model, "Annotation model should have be reset.");
}


#include "annotationmanagertest.moc"
QTEST_KDEMAIN(AnnotationManagerTest, GUI)
