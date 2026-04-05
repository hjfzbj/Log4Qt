/******************************************************************************
 *
 * This file is part of Log4Qt library.
 *
 * Copyright (C) 2007 - 2026 Log4Qt contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#ifndef LOG4QT_ABSTRACTLAYOUT_H
#define LOG4QT_ABSTRACTLAYOUT_H

#include "log4qtshared.h"
#include "log4qtsharedptr.h"

#include <QObject>

namespace Log4Qt
{

class LoggingEvent;

/*!
 * \brief The class Layout is the base class for all layouts.
 *
 * \note The ownership and lifetime of objects of this class are managed. See
 *       \ref Ownership "Object ownership" for more details.
 */
class LOG4QT_EXPORT AbstractLayout : public QObject
{
    Q_OBJECT

    /*!
     * The property holds the content type of the layout.
     *
     * \sa contentType()
     */
    Q_PROPERTY(QString footercontentType READ contentType)
    /*!
     * The property holds the footer used by the layout.
     *
     * \sa footer(), setFooter()
     */
    Q_PROPERTY(QString footer READ footer WRITE setFooter)
    /*!
     * The property holds the header used by the layout.
     *
     * \sa header(), setHeader()
     */
    Q_PROPERTY(QString header READ header WRITE setHeader)

public:
    AbstractLayout(QObject *parent = nullptr);
    virtual ~AbstractLayout();

public:
    [[nodiscard]] virtual QString contentType() const;
    [[nodiscard]] inline QString footer() const;
    [[nodiscard]] inline QString header() const;
    [[nodiscard]] inline QString name() const;
    inline void setFooter(const QString &footer);
    inline void setHeader(const QString &header);
    inline void setName(const QString &name);

    virtual void activateOptions();
    virtual QString format(const LoggingEvent &event) = 0;

    /*!
     * Returns true if this layout uses caller location information
     * (\c %F, \c %L, \c %M, \c %l in PatternLayout).
     *
     * Appenders and the logging infrastructure may query this to decide
     * whether capturing caller location is necessary for a given layout.
     * The default implementation returns \c false.
     */
    [[nodiscard]] virtual bool requiresLocation() const;

    /*!
     * Returns the end of line seperator for the operating system.
     *
     * Windows: \\r\\n
     * Mac: \\r
     * UNIX: \\n
     */
    static QString endOfLine();

    // Member variables
private:
    Q_DISABLE_COPY_MOVE(AbstractLayout)
    QString mFooter;
    QString mHeader;
};

inline QString AbstractLayout::footer() const
{
    return mFooter;
}

inline QString AbstractLayout::header() const
{
    return mHeader;
}

inline QString AbstractLayout::name() const
{
    return objectName();
}

inline void AbstractLayout::setFooter(const QString &footer)
{
    mFooter = footer;
}

inline void AbstractLayout::setHeader(const QString &header)
{
    mHeader = header;
}

inline void AbstractLayout::setName(const QString &name)
{
    setObjectName(name);
}

using LayoutSharedPtr = Log4QtSharedPtr<AbstractLayout>;

} // namespace Log4Qt

#endif // LOG4QT_ABSTRACTLAYOUT_H
