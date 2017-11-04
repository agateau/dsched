#ifndef LOGHIGHLIGHTER_H
#define LOGHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class LogHighlighter : public QSyntaxHighlighter
{
public:
    LogHighlighter(QTextDocument* document);

protected:
    void highlightBlock(const QString& text) override;
};

#endif /* LOGHIGHLIGHTER_H */
