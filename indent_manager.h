#pragma once

#include <QObject>
#include <QTextEdit>
#include <QString>
#include <QKeyEvent>

class IndentManager : public QObject {
    Q_OBJECT

public:
    enum Language {
        None,
        CPP,
        Python
    };

    explicit IndentManager(QTextEdit* editor, QObject* parent = nullptr);
    void setLanguage(Language lang);
    bool handleKeyPress(QKeyEvent* event);

private:
    bool handleTab();
    bool handleEnter();
    bool handleBackspace();
    QString getCurrentLineIndentation();
    bool shouldIncreaseIndent();
    bool shouldDecreaseIndent();
    int getIndentLevel(const QString& line);
    QString getIndentString() const;

    QTextEdit* editor;
    Language currentLanguage;
    const int spacesPerTab = 4;
    const QString cppBlockStart = "{";
    const QString cppBlockEnd = "}";
    const QStringList pythonBlockStarters = {
        "if", "for", "while", "def", "class", "with",
        "try", "except", "finally", "elif", "else"
    };
};
