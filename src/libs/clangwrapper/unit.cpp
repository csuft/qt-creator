/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#include "unit.h"

#include <clang-c/Index.h>

#include <QtCore/QByteArray>
#include <QtCore/QVector>
#include <QtCore/QSharedData>

namespace Clang {
namespace Internal {

class UnitData : public QSharedData
{
public:
    UnitData();
    UnitData(const QString &fileName);
    ~UnitData();

    void unload();
    bool isLoaded() const;

    CXIndex m_index;
    CXTranslationUnit m_tu;
    QByteArray m_fileName;
    QVector<QByteArray> m_compOptions;
    unsigned m_managOptions;
    UnsavedFiles m_unsaved;
};

} // Internal
} // Clang


using namespace Clang;
using namespace Internal;

UnitData::UnitData()
    : m_index(0)
    , m_tu(0)
{}

UnitData::UnitData(const QString &fileName)
    : m_index(clang_createIndex(/*excludeDeclsFromPCH*/ 1, /*displayDiagnostics*/ 0))
    , m_tu(0)
    , m_fileName(fileName.toUtf8())
{}

UnitData::~UnitData()
{
    clang_disposeIndex(m_index);
    unload();
}

void UnitData::unload()
{
    if (m_tu) {
        clang_disposeTranslationUnit(m_tu);
        m_tu = 0;
    }
}

bool UnitData::isLoaded() const
{
    return m_tu && m_index;
}

Unit::Unit()
    : m_data(new UnitData)
{}

Unit::Unit(const QString &fileName)
    : m_data(new UnitData(fileName))
{}

Unit::Unit(const Unit &unit)
    : m_data(unit.m_data)
{}

Unit &Unit::operator =(const Unit &unit)
{
    if (this != &unit)
        m_data = unit.m_data;
    return *this;
}

Unit::~Unit()
{}

const QString Unit::fileName() const
{
    return m_data->m_fileName;
}

bool Unit::isValid() const
{
    return m_data->isLoaded();
}

void Unit::invalidate()
{
    m_data->unload();
}

void Unit::setCompilationOptions(const QStringList &compOptions)
{
    m_data->m_compOptions.resize(compOptions.size());
    foreach (const QString &option, compOptions)
        m_data->m_compOptions.append(option.toUtf8());
}

void Unit::setUnsavedFiles(const UnsavedFiles &unsavedFiles)
{
    m_data->m_unsaved = unsavedFiles;
}

void Unit::setManagementOptions(unsigned managementOptions)
{
    m_data->m_managOptions = managementOptions;
}

void Unit::parse()
{
    m_data->unload();

    const char **argv = new const char*[m_data->m_compOptions.size()];
    for (int i = 0; i < m_data->m_compOptions.size(); ++i)
        argv[i] = m_data->m_compOptions.at(i).constData();

    m_data->m_tu = clang_parseTranslationUnit(m_data->m_index,
                                              m_data->m_fileName.constData(),
                                              argv, m_data->m_compOptions.size(),
                                              // @TODO: Extract UnsavedFileData...
                                              0, 0,
                                              m_data->m_managOptions);
    delete[] argv;
}

void Unit::reparse()
{
    // @TODO
}

void Unit::create()
{
    // @TODO
}

void Unit::createFromSourceFile()
{
    // @TODO
}

CXFile Unit::getFile() const
{
    return clang_getFile(m_data->m_tu, m_data->m_fileName.constData());
}

CXCursor Unit::getCursor(const CXSourceLocation &location) const
{
    return clang_getCursor(m_data->m_tu, location);
}

CXSourceLocation Unit::getLocation(const CXFile &file, unsigned line, unsigned column) const
{
    return clang_getLocation(m_data->m_tu, file, line, column);
}

CXCursor Unit::getTranslationUnitCursor() const
{
    return clang_getTranslationUnitCursor(m_data->m_tu);
}

CXString Unit::getTranslationUnitSpelling() const
{
    return clang_getTranslationUnitSpelling(m_data->m_tu);
}

void Unit::getInclusions(CXInclusionVisitor visitor, CXClientData clientData) const
{
    clang_getInclusions(m_data->m_tu, visitor, clientData);
}

unsigned Unit::getNumDiagnostics() const
{
    return clang_getNumDiagnostics(m_data->m_tu);
}

CXDiagnostic Unit::getDiagnostic(unsigned index) const
{
    return clang_getDiagnostic(m_data->m_tu, index);
}
