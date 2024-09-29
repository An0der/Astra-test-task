/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QFileSystemModel>
#include <QFileIconProvider>
#include <QScreen>
#include <QScroller>
#include <QTreeView>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QtDebug>
#include <QLineEdit>
#include <QWidget>
#include <QVBoxLayout>
#include <iostream>
#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QRegularExpression>

int main(int argc, char *argv[])

{
    QApplication app(argc, argv);
    app.setApplicationVersion(QT_VERSION_STR);

    QCommandLineParser parser;
    parser.setApplicationDescription("Qt Dir View Example");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption dontUseCustomDirectoryIconsOption("c", "Set QFileSystemModel::DontUseCustomDirectoryIcons");
    parser.addOption(dontUseCustomDirectoryIconsOption);
    QCommandLineOption dontWatchOption("w", "Set QFileSystemModel::DontWatch");
    parser.addOption(dontWatchOption);
    parser.addPositionalArgument("directory", "The directory to start in.");
    parser.process(app);

    // user's home path
    const QString userHomePath = QDir::homePath();
    QString rootPath = parser.positionalArguments().isEmpty() ?
                      userHomePath :
                      parser.positionalArguments().first();
   // if path is not valid sets user's home dir as root path
   if (!QDir(rootPath).exists())
       rootPath = userHomePath;

    QFileSystemModel model;
    model.setRootPath("");
    // binaries aren't displayed...
    model.setFilter(QDir::AllDirs|QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Hidden|QDir::System);
    if (parser.isSet(dontUseCustomDirectoryIconsOption))
        model.setOption(QFileSystemModel::DontUseCustomDirectoryIcons);
    if (parser.isSet(dontWatchOption))
        model.setOption(QFileSystemModel::DontWatchForChanges);


    // Proxy filtering model for filter behavior
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel;
    proxyModel->setRecursiveFilteringEnabled(true);
    // doesn't work if it's used here
    //proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(0);
    proxyModel->setSourceModel(&model);

    // folder itself is not displayed...
    QTreeView* tree = new QTreeView;
    tree->setModel(proxyModel);
    // Demonstrating look and feel features
    tree->setAnimated(false);
    tree->setIndentation(20);
    tree->setSortingEnabled(true);
    tree->setColumnWidth(0, tree->width() / 2);

    const QModelIndex rootIndex = proxyModel->mapFromSource(model.index(QDir::cleanPath(rootPath)));
    if (rootIndex.isValid())
        tree->setRootIndex(rootIndex);

    // Make it flickable on touchscreens
    QScroller::grabGesture(tree, QScroller::TouchGesture);

    QLineEdit *lineEdit = new QLineEdit;
    QString placeHolderText = "Find";
    lineEdit->setPlaceholderText(placeHolderText);

    QObject::connect(lineEdit, &QLineEdit::textChanged, [lineEdit, proxyModel, tree]()
    {
        QString text = lineEdit->text();
        proxyModel->setFilterRegularExpression(QRegularExpression(text));
        // should set it here
        proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

        if (!text.isEmpty())
            tree->expandAll();
        else
            tree->collapseAll();
    });
//    QObject::connect(lineEdit, SIGNAL(textChanged(const QString &)), proxyModel, SLOT(setFilterWildcard(const QString &)));

    QWidget *window = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(window);
    const QSize availableSize = window->screen()->availableGeometry().size();
    window->resize(availableSize / 2);
    layout->addWidget(lineEdit);
    layout->addWidget(tree);
    window->setWindowTitle("Dir View");
    window->show();

    return app.exec();
}
