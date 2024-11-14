#pragma once

#include <QObject>
#include <QTextEdit>
#include <QKeyEvent>
#include "custom_editor.h"

class IndentManager : public QObject {
    Q_OBJECT

public:
    enum class Language {
        None,
        CPP,
        Python
    };

    explicit IndentManager(CustomEditor* editor, QObject* parent = nullptr);
    void setLanguage(Language lang);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void handleKeyPress(QKeyEvent* event);
    void handleReturn();
    void handleTab();
    void handleBackspace();
    QString getCurrentIndentation();
    int getIndentationWidth();
    bool shouldIncreaseIndent();

    CustomEditor* editor;
    Language language;
};
