#include "code_highlighter.h"
#include <QApplication>
#include <QStyleHints>

CodeHighlighter::CodeHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
    , currentLanguage(None)
{
    cppCommentStartExp = QRegularExpression(QStringLiteral("/\\*"));
    cppCommentEndExp = QRegularExpression(QStringLiteral("\\*/"));
}

void CodeHighlighter::setupFormats(bool isDarkMode) {
    // Color scheme based on theme
    QColor keywordColor = isDarkMode ? QColor("#569CD6") : QColor("#0000FF");
    QColor classColor = isDarkMode ? QColor("#4EC9B0") : QColor("#2B91AF");
    QColor commentColor = isDarkMode ? QColor("#6A9955") : QColor("#008000");
    QColor stringColor = isDarkMode ? QColor("#CE9178") : QColor("#A31515");
    QColor functionColor = isDarkMode ? QColor("#DCDCAA") : QColor("#795E26");
    QColor numberColor = isDarkMode ? QColor("#B5CEA8") : QColor("#098658");
    QColor preprocessorColor = isDarkMode ? QColor("#C586C0") : QColor("#AF00DB");
    QColor decoratorColor = isDarkMode ? QColor("#569CD6") : QColor("#0000FF");

    // Setup formats
    keywordFormat.setForeground(keywordColor);
    keywordFormat.setFontWeight(QFont::Bold);

    classFormat.setForeground(classColor);

    singleLineCommentFormat.setForeground(commentColor);
    multiLineCommentFormat.setForeground(commentColor);

    quotationFormat.setForeground(stringColor);
    
    functionFormat.setForeground(functionColor);
    
    numberFormat.setForeground(numberColor);
    
    preprocessorFormat.setForeground(preprocessorColor);
    
    decoratorFormat.setForeground(decoratorColor);
}

void CodeHighlighter::setupCPPRules() {
    rules.clear();

    // Keywords
    const QString keywordPatterns[] = {
        QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"),
        QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"),
        QStringLiteral("\\bfriend\\b"), QStringLiteral("\\binline\\b"),
        QStringLiteral("\\bnamespace\\b"), QStringLiteral("\\boperator\\b"),
        QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"),
        QStringLiteral("\\bpublic\\b"), QStringLiteral("\\bsignals\\b"),
        QStringLiteral("\\bslots\\b"), QStringLiteral("\\bstatic\\b"),
        QStringLiteral("\\bstruct\\b"), QStringLiteral("\\btemplate\\b"),
        QStringLiteral("\\btypedef\\b"), QStringLiteral("\\btypename\\b"),
        QStringLiteral("\\bunion\\b"), QStringLiteral("\\bvirtual\\b"),
        QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bbreak\\b"),
        QStringLiteral("\\bcase\\b"), QStringLiteral("\\bcatch\\b"),
        QStringLiteral("\\bcontinue\\b"), QStringLiteral("\\bdefault\\b"),
        QStringLiteral("\\bdelete\\b"), QStringLiteral("\\bdo\\b"),
        QStringLiteral("\\belse\\b"), QStringLiteral("\\bfor\\b"),
        QStringLiteral("\\bgoto\\b"), QStringLiteral("\\bif\\b"),
        QStringLiteral("\\bnew\\b"), QStringLiteral("\\breturn\\b"),
        QStringLiteral("\\bswitch\\b"), QStringLiteral("\\btry\\b"),
        QStringLiteral("\\bwhile\\b"), QStringLiteral("\\bauto\\b"),
        QStringLiteral("\\bbool\\b"), QStringLiteral("\\bchar\\b"),
        QStringLiteral("\\bdouble\\b"), QStringLiteral("\\bfloat\\b"),
        QStringLiteral("\\bint\\b"), QStringLiteral("\\blong\\b"),
        QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsigned\\b"),
        QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\bvoid\\b"),
        QStringLiteral("\\boverride\\b"), QStringLiteral("\\bfinal\\b"),
        QStringLiteral("\\bnullptr\\b"), QStringLiteral("\\btrue\\b"),
        QStringLiteral("\\bfalse\\b"), QStringLiteral("\\bthis\\b")
    };

    for (const QString& pattern : keywordPatterns) {
        rules.append({QRegularExpression(pattern), keywordFormat});
    }

    // Class names (after class or struct keyword)
    rules.append({
        QRegularExpression(QStringLiteral("\\b(?:class|struct)\\s+(\\w+)\\b")),
        classFormat
    });

    // Single-line comments
    rules.append({
        QRegularExpression(QStringLiteral("//[^\n]*")),
        singleLineCommentFormat
    });

    // Quotation
    rules.append({
        QRegularExpression(QStringLiteral("\".*\"")),
        quotationFormat
    });

    // Functions
    rules.append({
        QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()")),
        functionFormat
    });

    // Numbers
    rules.append({
        QRegularExpression(QStringLiteral("\\b\\d+\\.?\\d*\\b")),
        numberFormat
    });

    // Preprocessor
    rules.append({
        QRegularExpression(QStringLiteral("#[a-zA-Z_][a-zA-Z0-9_]*\\b")),
        preprocessorFormat
    });
}

void CodeHighlighter::setupPythonRules() {
    rules.clear();

    // Keywords
    const QString keywordPatterns[] = {
        QStringLiteral("\\bFalse\\b"), QStringLiteral("\\bNone\\b"),
        QStringLiteral("\\bTrue\\b"), QStringLiteral("\\band\\b"),
        QStringLiteral("\\bas\\b"), QStringLiteral("\\bassert\\b"),
        QStringLiteral("\\bbreak\\b"), QStringLiteral("\\bclass\\b"),
        QStringLiteral("\\bcontinue\\b"), QStringLiteral("\\bdef\\b"),
        QStringLiteral("\\bdel\\b"), QStringLiteral("\\belif\\b"),
        QStringLiteral("\\belse\\b"), QStringLiteral("\\bexcept\\b"),
        QStringLiteral("\\bfinally\\b"), QStringLiteral("\\bfor\\b"),
        QStringLiteral("\\bfrom\\b"), QStringLiteral("\\bglobal\\b"),
        QStringLiteral("\\bif\\b"), QStringLiteral("\\bimport\\b"),
        QStringLiteral("\\bin\\b"), QStringLiteral("\\bis\\b"),
        QStringLiteral("\\blambda\\b"), QStringLiteral("\\bnonlocal\\b"),
        QStringLiteral("\\bnot\\b"), QStringLiteral("\\bor\\b"),
        QStringLiteral("\\bpass\\b"), QStringLiteral("\\braise\\b"),
        QStringLiteral("\\breturn\\b"), QStringLiteral("\\btry\\b"),
        QStringLiteral("\\bwhile\\b"), QStringLiteral("\\bwith\\b"),
        QStringLiteral("\\byield\\b")
    };

    for (const QString& pattern : keywordPatterns) {
        rules.append({QRegularExpression(pattern), keywordFormat});
    }

    // Class names
    rules.append({
        QRegularExpression(QStringLiteral("\\bclass\\s+(\\w+)\\b")),
        classFormat
    });

    // Single-line comments
    rules.append({
        QRegularExpression(QStringLiteral("#[^\n]*")),
        singleLineCommentFormat
    });

    // Decorators
    rules.append({
        QRegularExpression(QStringLiteral("@\\w+\\b")),
        decoratorFormat
    });

    // String literals (single and double quotes)
    rules.append({
        QRegularExpression(QStringLiteral("(['\"]).*\\1")),
        quotationFormat
    });

    // Functions
    rules.append({
        QRegularExpression(QStringLiteral("\\bdef\\s+(\\w+)\\b")),
        functionFormat
    });

    // Numbers
    rules.append({
        QRegularExpression(QStringLiteral("\\b\\d+\\.?\\d*\\b")),
        numberFormat
    });
}

void CodeHighlighter::setLanguage(Language lang) {
    currentLanguage = lang;
    setupFormats(QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark);
    
    switch (currentLanguage) {
        case CPP:
            setupCPPRules();
            break;
        case Python:
            setupPythonRules();
            break;
        default:
            rules.clear();
            break;
    }
    
    rehighlight();
}

void CodeHighlighter::updateTheme(bool isDarkMode) {
    setupFormats(isDarkMode);
    if (currentLanguage != None) {
        setLanguage(currentLanguage);  // This will also call rehighlight()
    }
}

void CodeHighlighter::highlightBlock(const QString& text) {
    // Apply regular expression rules
    for (const HighlightRule& rule : rules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Handle multi-line comments for C++
    if (currentLanguage == CPP) {
        setCurrentBlockState(0);
        int startIndex = 0;
        
        if (previousBlockState() != 1) {
            startIndex = text.indexOf(cppCommentStartExp);
        }

        while (startIndex >= 0) {
            QRegularExpressionMatch match = cppCommentEndExp.match(text, startIndex);
            int endIndex = match.capturedStart();
            int commentLength;
            
            if (endIndex == -1) {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            } else {
                commentLength = endIndex - startIndex + match.capturedLength();
            }
            
            setFormat(startIndex, commentLength, multiLineCommentFormat);
            startIndex = text.indexOf(cppCommentStartExp, startIndex + commentLength);
        }
    }
}
