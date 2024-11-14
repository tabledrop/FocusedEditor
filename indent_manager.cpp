#include "indent_manager.h"
#include <QTextCursor>
#include <QTextBlock>
#include <QRegularExpression>

IndentManager::IndentManager(QTextEdit* editor, QObject* parent)
    : QObject(parent)
    , editor(editor)
    , currentLanguage(None)
{
}

void IndentManager::setLanguage(Language lang) {
    currentLanguage = lang;
}

bool IndentManager::handleKeyPress(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Tab:
            return handleTab();
        case Qt::Key_Return:
        case Qt::Key_Enter:
            return handleEnter();
        case Qt::Key_Backspace:
            return handleBackspace();
        default:
            return false;
    }
}

QString IndentManager::getCurrentLineIndentation() {
    QTextCursor cursor = editor->textCursor();
    QString line = cursor.block().text();
    int indentCount = 0;
    
    for (QChar c : line) {
        if (c == ' ') {
            indentCount++;
        } else if (c == '\t') {
            indentCount += spacesPerTab;
        } else {
            break;
        }
    }
    
    return QString(indentCount, ' ');
}

bool IndentManager::shouldIncreaseIndent() {
    QTextCursor cursor = editor->textCursor();
    QString line = cursor.block().text().trimmed();
    
    if (currentLanguage == CPP) {
        return line.endsWith(cppBlockStart);
    } else if (currentLanguage == Python) {
        for (const QString& starter : pythonBlockStarters) {
            if (line.startsWith(starter) && line.endsWith(":")) {
                return true;
            }
        }
    }
    
    return false;
}

bool IndentManager::shouldDecreaseIndent() {
    QTextCursor cursor = editor->textCursor();
    QString line = cursor.block().text().trimmed();
    
    if (currentLanguage == CPP) {
        return line.startsWith(cppBlockEnd);
    } else if (currentLanguage == Python) {
        static QRegularExpression elsePattern("^(else|elif|except|finally)\\b");
        return elsePattern.match(line).hasMatch();
    }
    
    return false;
}

int IndentManager::getIndentLevel(const QString& line) {
    int spaces = 0;
    for (QChar c : line) {
        if (c == ' ') {
            spaces++;
        } else if (c == '\t') {
            spaces += spacesPerTab;
        } else {
            break;
        }
    }
    return spaces;
}

QString IndentManager::getIndentString() const {
    return QString(spacesPerTab, ' ');
}

bool IndentManager::handleTab() {
    if (editor->textCursor().hasSelection()) {
        // Indent selected lines
        QTextCursor cursor = editor->textCursor();
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd();
        
        cursor.setPosition(start);
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.beginEditBlock();
        
        while (cursor.position() <= end) {
            cursor.insertText(getIndentString());
            if (!cursor.movePosition(QTextCursor::NextBlock)) {
                break;
            }
        }
        
        cursor.endEditBlock();
        return true;
    } else {
        // Insert spaces at cursor
        editor->textCursor().insertText(getIndentString());
        return true;
    }
}

bool IndentManager::handleEnter() {
    QTextCursor cursor = editor->textCursor();
    QString currentIndent = getCurrentLineIndentation();
    QString additionalIndent;
    
    if (shouldIncreaseIndent()) {
        additionalIndent = getIndentString();
    }
    
    cursor.beginEditBlock();
    cursor.insertText("\n" + currentIndent + additionalIndent);
    cursor.endEditBlock();
    
    return true;
}

bool IndentManager::handleBackspace() {
    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection()) {
        return false;  // Let default handling work
    }
    
    QString line = cursor.block().text();
    int position = cursor.positionInBlock();
    
    // Check if we're in the indentation area
    bool inIndent = true;
    for (int i = 0; i < position; ++i) {
        if (line[i] != ' ' && line[i] != '\t') {
            inIndent = false;
            break;
        }
    }
    
    if (inIndent && position > 0) {
        // Delete up to the previous tab stop
        int spacesToRemove = position % spacesPerTab;
        if (spacesToRemove == 0) {
            spacesToRemove = spacesPerTab;
        }
        
        cursor.beginEditBlock();
        for (int i = 0; i < spacesToRemove; ++i) {
            cursor.deletePreviousChar();
        }
        cursor.endEditBlock();
        return true;
    }
    
    return false;
}
