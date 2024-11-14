#include "editor_window.h"
#include "preferences_dialog.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QShortcut>
#include <QKeySequence>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFileInfo>
#include <QCloseEvent>
#include <QPalette>
#include <QStyleHints>
#include <QApplication>
#include <QTextBlock>
#include <QScrollBar>
#include <QSettings>

EditorWindow::EditorWindow(QWidget* parent)
    : QMainWindow(parent)
    , unsavedChanges(false)
    , currentZoom(13)  // Initialize directly instead of using defaultFontSize
    , showingSplash(false)
{
    setMinimumSize(400, 300);
    
    // Create central widget and layout
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    
    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->setContentsMargins(11, 11, 11, 11);  
    layout->setSpacing(0);
    
    // Create and configure editor
    editor = new QTextEdit(this);
    editor->setFrameStyle(0);  // Remove frame
    
    // Install event filters
    editor->viewport()->installEventFilter(this);
    editor->installEventFilter(this);  // Add event filter to the editor itself
    
    // Initial theme setup (this will also set the font from settings)
    updateTheme();
    
    // Add editor to layout
    layout->addWidget(editor);
    
    // Initialize UI elements
    initUI();
    setupShortcuts();
    
    // Show splash screen
    showSplashScreen();
    
    // Connect to system theme changes
    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged,
            this, &EditorWindow::updateTheme);
}

void EditorWindow::initUI() {
    // Set window properties
    setWindowTitle("Focused Editor");
    resize(800, 600);

    // Connect text changed signal
    connect(editor, &QTextEdit::textChanged, this, &EditorWindow::handleTextChanged);
}

void EditorWindow::showSplashScreen() {
    if (!currentFile.isEmpty()) return;
    
    editor->clear();
    editor->setReadOnly(true);
    showingSplash = true;
    
    QTextCursor cursor = editor->textCursor();
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignCenter);
    
    cursor.movePosition(QTextCursor::Start);
    
    // Insert each line with proper centering
    cursor.insertBlock(blockFormat);
    cursor.insertText("Welcome to Focused Editor");
    
    cursor.insertBlock(blockFormat);
    cursor.insertText("\n");
    
    cursor.insertBlock(blockFormat);
    cursor.insertText("A minimalist text editor for distraction-free coding.");
    
    cursor.insertBlock(blockFormat);
    cursor.insertText("\n");
    
    cursor.insertBlock(blockFormat);
    cursor.insertText("Press Cmd+O to open a file");
    
    cursor.insertBlock(blockFormat);
    cursor.insertText("- or -");
    
    cursor.insertBlock(blockFormat);
    cursor.insertText("Start typing to create a new file");
    
    editor->setTextCursor(cursor);
}

void EditorWindow::hideSplashScreen() {
    if (!showingSplash) return;
    
    editor->clear();
    editor->setReadOnly(false);
    showingSplash = false;
}

void EditorWindow::handleTextChanged() {
    if (!showingSplash && !unsavedChanges) {
        unsavedChanges = true;
        updateTitle();
    }
}

void EditorWindow::setupShortcuts() {
    // Save
    auto saveShortcut = new QShortcut(QKeySequence::Save, this);
    connect(saveShortcut, &QShortcut::activated, this, &EditorWindow::saveFile);

    // Save As
    auto saveAsShortcut = new QShortcut(QKeySequence::SaveAs, this);
    connect(saveAsShortcut, &QShortcut::activated, this, &EditorWindow::saveFileAs);

    // Open
    auto openShortcut = new QShortcut(QKeySequence::Open, this);
    connect(openShortcut, &QShortcut::activated, this, &EditorWindow::openFile);

    // Fullscreen
    auto fullscreenShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F), this);
    connect(fullscreenShortcut, &QShortcut::activated, this, &EditorWindow::toggleFullscreen);

    // Quit
    auto quitShortcut = new QShortcut(QKeySequence::Quit, this);
    connect(quitShortcut, &QShortcut::activated, this, &QWidget::close);

    // Zoom shortcuts
    auto zoomInShortcut = new QShortcut(QKeySequence(tr("Ctrl+=")), this);
    connect(zoomInShortcut, &QShortcut::activated, this, &EditorWindow::zoomIn);
    
    // Alternative shortcut for plus key
    auto zoomInShortcut2 = new QShortcut(QKeySequence(tr("Ctrl++")), this);
    connect(zoomInShortcut2, &QShortcut::activated, this, &EditorWindow::zoomIn);

    auto zoomOutShortcut = new QShortcut(QKeySequence(tr("Ctrl+-")), this);
    connect(zoomOutShortcut, &QShortcut::activated, this, &EditorWindow::zoomOut);

    auto resetZoomShortcut = new QShortcut(QKeySequence(tr("Ctrl+0")), this);
    connect(resetZoomShortcut, &QShortcut::activated, this, &EditorWindow::resetZoom);
    
    // Preferences shortcut
    auto preferencesShortcut = new QShortcut(QKeySequence::Preferences, this);
    connect(preferencesShortcut, &QShortcut::activated, this, &EditorWindow::showPreferences);
}

void EditorWindow::showPreferences() {
    PreferencesDialog dialog(editor->font(), this);
    
    if (dialog.exec() == QDialog::Accepted) {
        QFont newFont = dialog.getSelectedFont();
        
        // Save font settings before recreating editor
        QSettings settings("Focused Editor", "Editor");
        settings.setValue("font/family", newFont.family());
        settings.setValue("font/size", newFont.pointSize());
        
        // Save current state
        QString content = editor->toPlainText();
        int scrollValue = editor->verticalScrollBar()->value();
        bool wasReadOnly = editor->isReadOnly();
        bool wasShowingSplash = showingSplash;
        
        // Remove event filters and disconnect signals from old editor
        editor->viewport()->removeEventFilter(this);
        editor->removeEventFilter(this);
        disconnect(editor, &QTextEdit::textChanged, this, &EditorWindow::handleTextChanged);
        
        // Create new editor with desired font
        QTextEdit* newEditor = new QTextEdit(this);
        newEditor->setFrameStyle(0);
        newEditor->setFont(newFont);
        newEditor->document()->setDefaultFont(newFont);
        newEditor->setPlainText(content);
        
        // Copy settings from old editor
        newEditor->verticalScrollBar()->setValue(scrollValue);
        newEditor->setReadOnly(wasReadOnly);
        
        // Replace old editor in layout
        QLayout* layout = centralWidget()->layout();
        layout->replaceWidget(editor, newEditor);
        
        // Clean up old editor
        editor->deleteLater();
        editor = newEditor;
        
        // Set up event filters and signals for new editor
        editor->viewport()->installEventFilter(this);
        editor->installEventFilter(this);
        connect(editor, &QTextEdit::textChanged, this, &EditorWindow::handleTextChanged);
        
        currentZoom = newFont.pointSize();
        showingSplash = wasShowingSplash;
        
        // Ensure editor is properly focused
        editor->setFocus();
        
        // Move cursor to end
        QTextCursor cursor = editor->textCursor();
        cursor.movePosition(QTextCursor::End);
        editor->setTextCursor(cursor);
        
        // Update theme to ensure proper styling
        updateTheme();
    }
}

void EditorWindow::updateTheme() {
    bool isDarkMode = QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    
    QString backgroundColor = isDarkMode ? "#1E1E1E" : "#FFFFFF";
    QString textColor = isDarkMode ? "#D4D4D4" : "#000000";
    QString scrollbarBg = isDarkMode ? "#2D2D2D" : "#F0F0F0";
    QString scrollbarHandle = isDarkMode ? "#4A4A4A" : "#CCCCCC";
    
    // Load saved font settings
    QSettings settings("Focused Editor", "Editor");
    QString fontFamily = settings.value("font/family", "Menlo").toString();
    int fontSize = settings.value("font/size", defaultFontSize).toInt();
    
    QFont font(fontFamily, fontSize);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    
    // Apply font to editor
    editor->setFont(font);
    editor->document()->setDefaultFont(font);
    currentZoom = fontSize;
    
    editor->setStyleSheet(QString(R"(
        QTextEdit {
            background-color: %1;
            border: none;
            color: %2;
            padding: 20px;
        }
        QScrollBar:vertical {
            width: 8px;
            background: %3;
        }
        QScrollBar::handle:vertical {
            background: %4;
            border-radius: 4px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical,
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: none;
            border: none;
        }
        QScrollBar:horizontal {
            height: 8px;
            background: %3;
        }
        QScrollBar::handle:horizontal {
            background: %4;
            border-radius: 4px;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal,
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
            background: none;
            border: none;
        }
    )")
    .arg(backgroundColor)
    .arg(textColor)
    .arg(scrollbarBg)
    .arg(scrollbarHandle));
    
    // Update the application-wide palette for consistent theming
    QPalette palette = QApplication::palette();
    palette.setColor(QPalette::Window, QColor(backgroundColor));
    palette.setColor(QPalette::WindowText, QColor(textColor));
    palette.setColor(QPalette::Base, QColor(backgroundColor));
    palette.setColor(QPalette::Text, QColor(textColor));
    QApplication::setPalette(palette);
}

void EditorWindow::updateTitle() {
    QString title = "Focused Editor";
    if (!currentFile.isEmpty()) {
        QFileInfo fileInfo(currentFile);
        title = fileInfo.fileName() + " - " + title;
    }
    if (unsavedChanges) {
        title = "*" + title;
    }
    setWindowTitle(title);
}

void EditorWindow::saveFile() {
    if (currentFile.isEmpty()) {
        saveFileAs();
    } else {
        saveToFile(currentFile);
    }
}

void EditorWindow::saveFileAs() {
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Save File",
        QString(),
        "Text Files (*.txt);;All Files (*)");
    
    if (!filePath.isEmpty()) {
        saveToFile(filePath);
    }
}

bool EditorWindow::saveToFile(const QString& filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << editor->toPlainText();
        currentFile = filePath;
        unsavedChanges = false;
        updateTitle();
        return true;
    } else {
        QMessageBox::critical(this, "Error", "Could not save file: " + file.errorString());
        return false;
    }
}

void EditorWindow::openFile() {
    if (maybeSave()) {
        QString filePath = QFileDialog::getOpenFileName(
            this,
            "Open File",
            QString(),
            "Text Files (*.txt);;All Files (*)");
        
        if (!filePath.isEmpty()) {
            loadFile(filePath);
        }
    }
}

bool EditorWindow::loadFile(const QString& filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        editor->setPlainText(stream.readAll());
        currentFile = filePath;
        unsavedChanges = false;
        updateTitle();
        return true;
    } else {
        QMessageBox::critical(this, "Error", "Could not open file: " + file.errorString());
        return false;
    }
}

bool EditorWindow::maybeSave() {
    if (!unsavedChanges) {
        return true;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Save Changes",
        "Do you want to save your changes?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    if (reply == QMessageBox::Save) {
        return saveFile(), true;
    } else if (reply == QMessageBox::Discard) {
        return true;
    } else {
        return false;
    }
}

void EditorWindow::toggleFullscreen() {
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void EditorWindow::zoomIn() {
    updateZoom(2);
}

void EditorWindow::zoomOut() {
    updateZoom(-2);
}

void EditorWindow::resetZoom() {
    currentZoom = 13;
    QFont font = editor->font();
    font.setPointSize(currentZoom);
    editor->setFont(font);
}

void EditorWindow::updateZoom(int delta) {
    int newSize = currentZoom + delta;
    if (newSize >= 8 && newSize <= 24) {
        currentZoom = newSize;
        QFont font = editor->font();
        font.setPointSize(currentZoom);
        editor->setFont(font);
    }
}

bool EditorWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == editor->viewport()) {
        if (event->type() == QEvent::Wheel) {
            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            if (wheelEvent->modifiers() & Qt::ControlModifier) {
                const int delta = wheelEvent->angleDelta().y();
                updateZoom(delta > 0 ? 2 : -2);
                return true;
            }
        }
    }
    
    // Handle keyboard events for the editor
    if (obj == editor && showingSplash) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            
            // Don't clear for modifier keys or special keys
            if (!keyEvent->text().isEmpty() && 
                !(keyEvent->modifiers() & (Qt::ControlModifier | Qt::MetaModifier))) {
                hideSplashScreen();
                
                // If it's a printable character, send it to the editor
                if (keyEvent->text()[0].isPrint()) {
                    editor->textCursor().insertText(keyEvent->text());
                }
                return true;
            }
        }
    }
    
    return QMainWindow::eventFilter(obj, event);
}

void EditorWindow::closeEvent(QCloseEvent* event) {
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}
