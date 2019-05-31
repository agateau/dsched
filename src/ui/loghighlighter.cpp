/*
 * Copyright 2017 Aurélien Gâteau <mail@agateau.com>
 *
 * This file is part of Dsched.
 *
 * Dsched is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
