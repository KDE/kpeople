/*
    Copyright (C) 2019 Aleix Pol i Gonzalez <aleixpol@blue-systems.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef KPEOPLE_ABSTRACT_EDITABLE_CONTACT_H
#define KPEOPLE_ABSTRACT_EDITABLE_CONTACT_H

#include "abstractcontact.h"
#include <kpeoplebackend/kpeoplebackend_export.h>

namespace KPeople
{
/**
 * @brief Additionally to @class AbstractContact features, it will allow us
 * to suggest the backend property values for a said key.
 *
 * @since 5.62
 * @internal
 */

class KPEOPLEBACKEND_EXPORT AbstractEditableContact : public AbstractContact
{
public:
    typedef QExplicitlySharedDataPointer<AbstractEditableContact> Ptr;
    typedef QList<AbstractEditableContact::Ptr> List;
    AbstractEditableContact();
    virtual ~AbstractEditableContact();

    /**
     * @p key @see AbstractContact for key identifiers
     * @p value suggested value
     *
     * @returns whether the change was successful
     */
    virtual bool setCustomProperty(const QString &key, const QVariant &value) = 0;

private:
    Q_DISABLE_COPY(AbstractEditableContact)
};

}

Q_DECLARE_METATYPE(KPeople::AbstractEditableContact::List)
Q_DECLARE_METATYPE(KPeople::AbstractEditableContact::Ptr)

#endif
