#ifndef LOG4QTDEFS_H
#define LOG4QTDEFS_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
#include <QtClassHelperMacros>
#endif

// Enable Qt 6.4+ string literal operator for better performance
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
#include <QString>
using namespace Qt::StringLiterals;
// Use modern u"..."_s syntax for Qt 6.4+
#define LOG4QT_LITERAL(str) u##str##_s
#else
// Fallback to QStringLiteral for older Qt versions
#define LOG4QT_LITERAL(str) QStringLiteral(str)
#endif

// use Q_DISABLE_COPY for Qt version prior to 5.13.0
#ifndef Q_DISABLE_COPY_MOVE
#define Q_DISABLE_COPY_MOVE(Class) \
Q_DISABLE_COPY(Class)
#endif

#endif // LOG4QTDEFS_H
