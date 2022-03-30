/*
    SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once

#include <QQuickImageProvider>

/**
 * @brief An image provider for displaying a contact's photo
 *
 * It is automatically available when importing the KPeople QML module.
 *
 * Image URIs for it can be retrieved from `KPeople.PersonData`'s `photoProviderUri` property,
 * or, if you don't have a `KPeople.PersonData` instance, from `KPeople.AvatarUtils.photoProviderUri(personUri)`.
 *
 * Example:
 * \code
 * Kirigami.Avatar {
 *     source: KPeople.AvatarUtils.photoProviderUri(personUri)
 * }
 * \endcode
 *
 * @since 5.93.0
 */
class AvatarImageProvider : public QQuickImageProvider
{
public:
    explicit AvatarImageProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;
};

class QmlAvatarUriHelper : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE static QString photoProviderUri(const QString &personUri);
};
