#include "indent_manager.h"
#include <QTextCursor>
#include <QTextBlock>
#include <QDebug>

IndentManager::IndentManager(CustomEditor* editor, QObject* parent)
    : QObject(parent)
    , editor(editor)
    , language(Language::None)
{
    editor->installEventFilter(this);
}

void IndentManager::setLanguage(Language lang) {
    language = lang;
}

bool IndentManager::eventFilter(QObject* obj, QEvent* event) {
    if (obj == editor && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        handleKeyPress(keyEvent);
        return false;  // Always let the editor handle the event too
    }
    return false;
}

void IndentManager::handleKeyPress(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            handleReturn();
            break;
        case Qt::Key_Tab:
            handleTab();
            break;
        case Qt::Key_Backspace:
            handleBackspace();
            break;
        default:
            break;
    }
}

void IndentManager::handleReturn() {
    if (language == Language::None) return;
    
    QTextCursor cursor = editor->textCursor();
    QString currentIndent = getCurrentIndentation();
    
    if (shouldIncreaseIndent()) {
        currentIndent += QString(getIndentationWidth(), ' ');
    }
    
    // Insert new line with indentation
    cursor.insertText("\n" + currentIndent);
    editor->setTextCursor(cursor);
}

void IndentManager::handleTab() {
    QTextCursor cursor = editor->textCursor();
    cursor.insertText(QString(getIndentationWidth(), ' '));
}

void IndentManager::handleBackspace() {
    QTextCursor cursor = editor->textCursor();
    QString currentLine = cursor.block().text();
    int column = cursor.columnNumber();
    
    // Check if we're at the start of a line with only spaces
    if (column > 0 && currentLine.left(column).trimmed().isEmpty()) {
        int spaces = getIndentationWidth();
        int spacesToRemove = column % spaces;
        if (spacesToRemove == 0) spacesToRemove = spaces;
        
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, spacesToRemove);
        cursor.removeSelectedText();
    }
}

QString IndentManager::getCurrentIndentation() {
    QTextCursor cursor = editor->textCursor();
    QString line = cursor.block().text();
    QString indent;
    
    for (QChar ch : line) {
        if (ch.isSpace()) {
            indent += ch;
        } else {
            break;
        }
    }
    
    return indent;
}

int IndentManager::getIndentationWidth() {
    return 4;  // Fixed 4-space indentation
}

bool IndentManager::shouldIncreaseIndent() {
    QTextCursor cursor = editor->textCursor();
    QString line = cursor.block().text().trimmed();
    
    if (language == Language::CPP) {
        return line.endsWith("{");
    } else if (language == Language::Python) {
        return line.endsWith(":");
    }
    
    return false;
}
