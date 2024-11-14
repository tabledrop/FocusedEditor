#include "editor_window.h"
#include "preferences_dialog.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QAction>
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
#include "code_highlighter.h"
#include "indent_manager.h"
#include "line_number_area.h"
#include "custom_editor.h"

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
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    // Create and configure editor
    editor = new CustomEditor(this);
    editor->setFrameStyle(0);  // Remove frame
    
    // Create syntax highlighter and indent manager
    highlighter = new CodeHighlighter(editor->document());
    indentManager = new IndentManager(editor, this);
    
    // Create line number area (initially hidden)
    lineNumberArea = new LineNumberArea(editor);
    lineNumberArea->setVisible(false);
    
    // Install event filters
    editor->viewport()->installEventFilter(this);
    editor->installEventFilter(this);
    
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
            
    // Connect editor signals
    connect(editor->document(), &QTextDocument::contentsChange,
            this, &EditorWindow::handleTextChanged);
            
    connect(editor, &QPlainTextEdit::updateRequest,
            this, &EditorWindow::updateLineNumberArea);
            
    connect(editor->document(), &QTextDocument::blockCountChanged,
            this, &EditorWindow::updateLineNumberAreaWidth);
}

void EditorWindow::setupShortcuts() {
    // File operations
    QAction* saveAction = new QAction(this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &EditorWindow::saveFile);
    addAction(saveAction);
    
    QAction* openAction = new QAction(this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &EditorWindow::openFile);
    addAction(openAction);
    
    // Preferences
    QAction* prefsAction = new QAction(this);
    prefsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Comma));
    connect(prefsAction, &QAction::triggered, this, &EditorWindow::showPreferences);
    addAction(prefsAction);
    
    // View operations
    QAction* fullscreenAction = new QAction(this);
    fullscreenAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
    connect(fullscreenAction, &QAction::triggered, this, &EditorWindow::toggleFullscreen);
    addAction(fullscreenAction);
    
    QAction* zoomInAction = new QAction(this);
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAction, &QAction::triggered, this, &EditorWindow::zoomIn);
    addAction(zoomInAction);
    
    QAction* zoomOutAction = new QAction(this);
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAction, &QAction::triggered, this, &EditorWindow::zoomOut);
    addAction(zoomOutAction);
    
    QAction* resetZoomAction = new QAction(this);
    resetZoomAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(resetZoomAction, &QAction::triggered, this, &EditorWindow::resetZoom);
    addAction(resetZoomAction);
    
    // Line numbers
    QAction* lineNumbersAction = new QAction(this);
    lineNumbersAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    connect(lineNumbersAction, &QAction::triggered, this, &EditorWindow::toggleLineNumbers);
    addAction(lineNumbersAction);
}

void EditorWindow::toggleLineNumbers() {
    if (lineNumberArea) {
        bool newVisible = !lineNumberArea->isVisible();
        lineNumberArea->setVisible(newVisible);
        updateLineNumberAreaWidth();
        if (newVisible) {
            lineNumberArea->update();
        }
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
        QPlainTextEdit {
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
    
    // Update line number area style
    if (lineNumberArea) {
        lineNumberArea->setStyleSheet(QString(R"(
            QWidget {
                background-color: %1;
                padding-top: 20px;
            }
        )").arg(isDarkMode ? "#1E1E1E" : "#F0F0F0"));
    }
    
    // Update the application-wide palette for consistent theming
    QPalette palette = QApplication::palette();
    palette.setColor(QPalette::Window, QColor(backgroundColor));
    palette.setColor(QPalette::WindowText, QColor(textColor));
    palette.setColor(QPalette::Base, QColor(backgroundColor));
    palette.setColor(QPalette::Text, QColor(textColor));
    QApplication::setPalette(palette);
    
    // Update syntax highlighter theme
    highlighter->updateTheme(isDarkMode);
}

void EditorWindow::initUI() {
    // Set window properties
    setWindowTitle("Focused Editor");
    resize(800, 600);

    // Connect signals
    connect(editor->document(), &QTextDocument::contentsChange,
            this, &EditorWindow::handleTextChanged);
            
    connect(editor, &QPlainTextEdit::updateRequest,
            this, &EditorWindow::updateLineNumberArea);
            
    connect(editor->document(), &QTextDocument::blockCountChanged,
            this, &EditorWindow::updateLineNumberAreaWidth);
}

void EditorWindow::updateLineNumberAreaWidth() {
    if (lineNumberArea && lineNumberArea->isVisible()) {
        int width = lineNumberArea->sizeHint().width();
        
        // Set viewport margins first
        editor->setCustomViewportMargins(width, 0, 0, 0);
        
        // Get viewport geometry and adjust for line numbers
        QRect rect = editor->viewport()->geometry();
        rect.setLeft(rect.left() - width);  // Move left to create space
        rect.setWidth(width);  // Set width for line numbers
        lineNumberArea->setGeometry(rect);
    } else {
        editor->setCustomViewportMargins(0, 0, 0, 0);
    }
}

void EditorWindow::updateLineNumberArea(const QRect& rect, int dy) {
    if (!lineNumberArea || !lineNumberArea->isVisible()) return;
    
    if (dy) {
        lineNumberArea->scroll(0, dy);
    } else {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    if (rect.contains(editor->viewport()->rect())) {
        updateLineNumberAreaWidth();
    }
}

void EditorWindow::showSplashScreen() {
    if (!currentFile.isEmpty()) return;
    
    editor->clear();
    editor->setReadOnly(true);
    showingSplash = true;
    unsavedChanges = false;  // Don't prompt to save splash screen
    
    // Create centered text format
    QTextBlockFormat centerFormat;
    centerFormat.setAlignment(Qt::AlignHCenter);
    
    // Create cursor and start with empty block for top margin
    QTextCursor cursor(editor->document());
    
    // Add top margin (about 30% of editor height)
    QTextBlockFormat topMarginFormat = centerFormat;
    topMarginFormat.setTopMargin(editor->height() * 0.3);
    cursor.setBlockFormat(topMarginFormat);
    
    // Welcome text
    cursor.insertText("Welcome to Focused Editor");
    cursor.insertBlock(centerFormat);
    cursor.insertText("\n");
    cursor.insertBlock(centerFormat);
    
    // Description
    cursor.insertText("A minimalist text editor for distraction-free coding.");
    cursor.insertBlock(centerFormat);
    cursor.insertText("\n");
    cursor.insertBlock(centerFormat);
    
    // Instructions
    cursor.insertText("Press Cmd+O to open a file");
    cursor.insertBlock(centerFormat);
    cursor.insertText("- or -");
    cursor.insertBlock(centerFormat);
    cursor.insertText("Start typing to create a new file");
    
    // Hide line numbers for splash screen
    if (lineNumberArea) {
        lineNumberArea->setVisible(false);
    }
    
    // Move cursor to start to avoid selection
    cursor.movePosition(QTextCursor::Start);
    editor->setTextCursor(cursor);
    
    // Force center alignment for the entire document
    QTextDocument* doc = editor->document();
    QTextOption opt = doc->defaultTextOption();
    opt.setAlignment(Qt::AlignHCenter);
    doc->setDefaultTextOption(opt);
}

void EditorWindow::hideSplashScreen() {
    if (!showingSplash) return;
    
    showingSplash = false;
    editor->clear();
    editor->setReadOnly(false);
    unsavedChanges = false;  // Reset changes flag when hiding splash
    
    // Reset text alignment to left
    QTextDocument* doc = editor->document();
    QTextOption opt = doc->defaultTextOption();
    opt.setAlignment(Qt::AlignLeft);
    doc->setDefaultTextOption(opt);
    
    // Show line numbers when exiting splash screen
    if (lineNumberArea) {
        lineNumberArea->setVisible(true);
        updateLineNumberAreaWidth();
    }
}

void EditorWindow::handleTextChanged() {
    if (!showingSplash) {  // Only mark changes when not showing splash screen
        // Mark as unsaved only if the content actually changed
        QString currentText = editor->toPlainText();
        if (!currentFile.isEmpty()) {
            QFile file(currentFile);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QString savedText = QTextStream(&file).readAll();
                unsavedChanges = (currentText != savedText);
                updateTitle();
                file.close();
            }
        } else {
            // For new files, mark as unsaved if there's any content
            unsavedChanges = !currentText.isEmpty();
            updateTitle();
        }
    }
}

void EditorWindow::updateSyntaxHighlighting() {
    if (!currentFile.isEmpty()) {
        QString extension = QFileInfo(currentFile).suffix().toLower();
        CodeHighlighter::Language hlLang = CodeHighlighter::None;
        IndentManager::Language indentLang = IndentManager::None;
        
        if (extension == "cpp" || extension == "h" || extension == "hpp" || extension == "c" || extension == "cc") {
            hlLang = CodeHighlighter::CPP;
            indentLang = IndentManager::CPP;
        } else if (extension == "py") {
            hlLang = CodeHighlighter::Python;
            indentLang = IndentManager::Python;
        }
        
        highlighter->setLanguage(hlLang);
        indentManager->setLanguage(indentLang);
    } else {
        highlighter->setLanguage(CodeHighlighter::None);
        indentManager->setLanguage(IndentManager::None);
    }
}

bool EditorWindow::saveFile() {
    qDebug() << "Save file triggered, current unsavedChanges:" << unsavedChanges;  // Debug output
    if (currentFile.isEmpty()) {
        saveFileAs();  // If no file path yet, prompt for save location
    } else {
        saveToFile(currentFile);  // Save to existing file
    }
    
    qDebug() << "Save completed, unsavedChanges:" << unsavedChanges;  // Debug output
    return true;
}

void EditorWindow::saveFileAs() {
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save File"),
        QString(),
        tr("All Files (*)")
    );
    
    if (!filePath.isEmpty()) {
        saveToFile(filePath);
    }
}

bool EditorWindow::saveToFile(const QString& filePath) {
    qDebug() << "Saving to file:" << filePath;  // Debug output
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot save file: ") + file.errorString());
        return false;
    }

    QString content = editor->toPlainText();
    QTextStream out(&file);
    out << content;
    file.close();
    
    // Update current file path and state
    currentFile = filePath;
    unsavedChanges = false;  // Reset unsaved changes flag
    
    // Update UI and language settings
    updateTitle();
    updateSyntaxHighlighting();
    
    qDebug() << "Save completed, unsavedChanges:" << unsavedChanges;  // Debug output
    return true;
}

void EditorWindow::openFile() {
    if (maybeSave()) {  // Only check for save if there are actual changes
        QString filePath = QFileDialog::getOpenFileName(
            this,
            "Open File",
            QString(),
            "All Files (*.*)"
        );
        
        if (!filePath.isEmpty()) {
            loadFile(filePath);
        }
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

void EditorWindow::showPreferences() {
    PreferencesDialog dialog(editor->font(), this);
    
    if (dialog.exec() == QDialog::Accepted) {
        QFont newFont = dialog.getSelectedFont();
        
        // Save font settings
        QSettings settings("Focused Editor", "Editor");
        settings.setValue("font/family", newFont.family());
        settings.setValue("font/size", newFont.pointSize());
        
        // Update editor font
        editor->setFont(newFont);
        editor->document()->setDefaultFont(newFont);
        currentZoom = newFont.pointSize();
        
        // Update theme to ensure proper styling
        updateTheme();
    }
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

bool EditorWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == editor) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            
            // Handle indentation if we're not showing splash screen
            if (!showingSplash && indentManager->handleKeyPress(keyEvent)) {
                return true;
            }
            
            // Handle splash screen key events
            if (showingSplash) {
                if (!keyEvent->text().isEmpty() && 
                    !(keyEvent->modifiers() & (Qt::ControlModifier | Qt::MetaModifier))) {
                    hideSplashScreen();
                    
                    if (keyEvent->text()[0].isPrint()) {
                        editor->textCursor().insertText(keyEvent->text());
                        return true;
                    }
                }
            }
        }
    }
    
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
    
    return QMainWindow::eventFilter(obj, event);
}

void EditorWindow::closeEvent(QCloseEvent* event) {
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void EditorWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    updateLineNumberAreaWidth();  // Update line number area on resize
}

void EditorWindow::loadFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Cannot open file: " + file.errorString());
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    
    // First hide splash screen (this will also clear readonly flag)
    hideSplashScreen();
    
    // Then set content and update state
    editor->setPlainText(content);
    currentFile = filePath;
    unsavedChanges = false;
    
    // Make sure editor is editable and has focus
    editor->setReadOnly(false);
    editor->moveCursor(QTextCursor::Start);
    editor->setFocus();
    
    // Update UI
    updateTitle();
    updateSyntaxHighlighting();
    
    // Show line numbers when loading a file
    if (lineNumberArea) {
        lineNumberArea->setVisible(true);
        updateLineNumberAreaWidth();
    }
}
