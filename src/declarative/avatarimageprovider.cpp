/*
    SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "avatarimageprovider.h"

#include <persondata.h>

#include "kpeopledeclarative_debug.h"

AvatarImageProvider::AvatarImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap AvatarImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    const auto decoded = QByteArray::fromBase64(id.toUtf8(), QByteArray::AbortOnBase64DecodingErrors);
    if (decoded.isEmpty()) {
        qCDebug(KPEOPLE_DECLARATIVE_LOG) << "AvatarImageProvider:" << id << "could not be decoded as a person uri";
        return {};
    }

    auto personUri = QString::fromUtf8(decoded);
    if (personUri.isEmpty()) {
        qCDebug(KPEOPLE_DECLARATIVE_LOG()) << "AvatarImageProvider:"
                                           << "passed person uri" << personUri << "was not valid utf8";
        return {};
    }

    KPeople::PersonData person(personUri);
    if (!person.isValid()) {
        qCDebug(KPEOPLE_DECLARATIVE_LOG()) << "AvatarImageProvider:"
                                           << "No contact found with person uri" << personUri;
        return {};
    }

    if (size) {
        *size = requestedSize;
    }

    return person.photo().scaled(requestedSize);
}

QString QmlAvatarUriHelper::photoProviderUri(const QString &personUri)
{
    return u"image://kpeople-avatar/" % QString::fromUtf8(personUri.toUtf8().toBase64());
}
