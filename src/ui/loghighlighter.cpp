#include "loghighlighter.h"

#include <QTextCharFormat>
#include <QRegularExpression>

static const char PATTERN[] = "^Dsched: [-T0-9:]+: ([A-Za-z]+).*";
static const char STARTING[] = "Starting";
static const char SUCCESS[] = "OK";

LogHighlighter::LogHighlighter(QTextDocument* document)
: QSyntaxHighlighter(document)
{
}

void LogHighlighter::highlightBlock(const QString& text)
{
    QTextCharFormat startingFormat;
    startingFormat.setForeground(Qt::darkBlue);

    QTextCharFormat successFormat;
    successFormat.setForeground(Qt::darkGreen);

    QTextCharFormat errorFormat;
    errorFormat.setForeground(Qt::red);

    QRegularExpression expression(PATTERN);

    QRegularExpressionMatch match;
    int index = text.indexOf(expression, 0, &match);
    while (index >= 0) {
        int length = match.capturedLength();
        QString word = match.captured(1);
        if (word == STARTING) {
            setFormat(index, length, startingFormat);
        } else if (word == SUCCESS) {
            setFormat(index, length, successFormat);
        } else {
            setFormat(index, length, errorFormat);
        }
        index = text.indexOf(expression, index + length, &match);
    }
}
