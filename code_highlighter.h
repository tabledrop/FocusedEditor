#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QHash>

class CodeHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    enum Language {
        None,
        CPP,
        Python
    };

    explicit CodeHighlighter(QTextDocument* parent = nullptr);
    void setLanguage(Language lang);
    void updateTheme(bool isDarkMode);

protected:
    void highlightBlock(const QString& text) override;

private:
    void setupCPPRules();
    void setupPythonRules();
    void setupFormats(bool isDarkMode);

    struct HighlightRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    Language currentLanguage;
    QVector<HighlightRule> rules;
    
    // Multi-line comment handling
    QRegularExpression cppCommentStartExp;
    QRegularExpression cppCommentEndExp;
    QTextCharFormat multiLineCommentFormat;

    // Common formats
    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat preprocessorFormat;  // For C/C++
    QTextCharFormat decoratorFormat;     // For Python
};
