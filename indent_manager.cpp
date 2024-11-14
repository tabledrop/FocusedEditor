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
            return handleCharacter(event);
    }
}

bool IndentManager::handleCharacter(QKeyEvent* event) {
    if (!isLanguageMode() || event->text().isEmpty()) {
        return false;
    }

    QString character = event->text();
    QTextCursor cursor = editor->textCursor();
    
    // Handle quotes specially
    if (character == "\"" || character == "'") {
        // Check if we should skip over existing quote
        if (!cursor.atEnd()) {
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            if (cursor.selectedText() == character) {
                // Move cursor over the existing quote
                cursor.clearSelection();
                editor->setTextCursor(cursor);
                return true;
            }
            cursor.movePosition(QTextCursor::PreviousCharacter);
        }
        // Always add pair for quotes
        insertMatchingPair(character, character);
        return true;
    }
    
    // Check if this is a closing character (except quotes)
    for (auto it = autoPairs.begin(); it != autoPairs.end(); ++it) {
        if (it.key() != "\"" && it.key() != "'" && // Skip quotes in this check
            character == it.value()) {  // If it's a closing character
            // Check if the next character is already this closing character
            if (!cursor.atEnd()) {
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                if (cursor.selectedText() == character) {
                    // Move cursor over the existing character instead of inserting
                    cursor.clearSelection();
                    editor->setTextCursor(cursor);
                    return true;
                }
                cursor.movePosition(QTextCursor::PreviousCharacter);
            }
            return false;  // Let the character be inserted normally
        }
    }
    
    // Check if this is an opening character that needs auto-pairing
    if (autoPairs.contains(character)) {
        if (character == "{" && currentLanguage == CPP) {
            formatBlock(character);
            return true;
        } else {
            insertMatchingPair(character, autoPairs[character]);
            return true;
        }
    }
    
    return false;
}

void IndentManager::insertMatchingPair(const QString& opening, const QString& closing) {
    QTextCursor cursor = editor->textCursor();
    
    // If text is selected, wrap it with the pair
    if (cursor.hasSelection()) {
        QString selectedText = cursor.selectedText();
        cursor.beginEditBlock();
        cursor.insertText(opening + selectedText + closing);
        cursor.endEditBlock();
    } else {
        cursor.beginEditBlock();
        cursor.insertText(opening + closing);
        cursor.movePosition(QTextCursor::Left);
        cursor.endEditBlock();
        editor->setTextCursor(cursor);
    }
}

void IndentManager::formatBlock(const QString& opening) {
    QTextCursor cursor = editor->textCursor();
    QString indent = getCurrentLineIndentation();
    QString additionalIndent = getIndentString();
    
    cursor.beginEditBlock();
    
    // Insert opening brace
    cursor.insertText(opening);
    
    // Insert newline and indented empty line
    cursor.insertText("\n" + indent + additionalIndent);
    
    // Store this position for later
    int contentPosition = cursor.position();
    
    // Insert newline and closing brace
    cursor.insertText("\n" + indent + "}");
    
    // Move cursor back to the indented empty line
    cursor.setPosition(contentPosition);
    
    cursor.endEditBlock();
    editor->setTextCursor(cursor);
}

bool IndentManager::handleEnter() {
    QTextCursor cursor = editor->textCursor();
    QString currentIndent = getCurrentLineIndentation();
    QString additionalIndent;
    
    // Check if we're between braces in C++
    if (currentLanguage == CPP) {
        QString line = cursor.block().text();
        int cursorPos = cursor.positionInBlock();
        
        // Check if we're between braces
        bool betweenBraces = false;
        if (cursorPos > 0 && cursorPos < line.length()) {
            if (line[cursorPos-1] == '{' && line[cursorPos] == '}') {
                betweenBraces = true;
            }
        }
        
        if (betweenBraces) {
            cursor.beginEditBlock();
            // Insert newline and indent for content
            cursor.insertText("\n" + currentIndent + getIndentString());
            // Insert newline and indent for closing brace
            cursor.insertText("\n" + currentIndent);
            // Move cursor back to content line
            cursor.movePosition(QTextCursor::Up);
            cursor.endEditBlock();
            editor->setTextCursor(cursor);
            return true;
        }
    }
    
    if (shouldIncreaseIndent()) {
        additionalIndent = getIndentString();
    }
    
    cursor.beginEditBlock();
    cursor.insertText("\n" + currentIndent + additionalIndent);
    cursor.endEditBlock();
    
    return true;
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
