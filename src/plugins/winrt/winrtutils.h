/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef WINRTUTILS_H
#define WINRTUTILS_H

#if defined(_MSC_VER) && _MSC_VER >= 1700
#  include <wrl.h>
#  include <windows.management.deployment.h>
#  include <windows.system.userprofile.h>
#  include <sddl.h>

namespace WinRt {
namespace Internal {

/* Utility methods for constructing WinRT types */

// Return an instance of the RT class so we don't need to use ref new
// usage: qNewRT<TypeName>(TypeNameString);
template<typename T, int n>
static inline T *qNewRt(const WCHAR (&id)[n])
{
    IInspectable *instance;
    if (FAILED(RoActivateInstance(Microsoft::WRL::Wrappers::HString::MakeReference(id).Get(), &instance)))
        return nullptr;
    return static_cast<T*>(instance);
}

// Release memory of IUnknown (to balance qNew from above)
static inline void qDeleteRt(IUnknown *t)
{
    t->Release();
}

} // namespace Internal
} // namespace WinRt

#endif // defined(_MSC_VER) && _MSC_VER >= 1700

#endif // WINRTUTILS_H