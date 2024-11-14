#include "editor_window.h"
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

EditorWindow::EditorWindow(QWidget* parent)
    : QMainWindow(parent)
    , unsavedChanges(false)
    , currentZoom(13)
{
    initUI();
    setupShortcuts();
    
    // Connect to system theme changes
    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged,
            this, &EditorWindow::updateTheme);
}

void EditorWindow::initUI() {
    // Create central widget and layout
    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    auto layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(20, 20, 20, 20);

    // Create and configure text editor
    editor = new QTextEdit(this);
    editor->setFrameStyle(0);  // Remove frame
    
    // Install event filter for wheel events
    editor->viewport()->installEventFilter(this);
    
    // Set monospace font with fallbacks
    QFont font;
    font.setFamilies(QStringList{"Menlo", "Monaco", "Courier New"});
    font.setFixedPitch(true);
    font.setPointSize(13);
    editor->setFont(font);
    
    // Initial theme setup
    updateTheme();

    // Add editor to layout
    layout->addWidget(editor);

    // Set window properties
    setWindowTitle("Focused Editor");
    resize(800, 600);

    // Connect text changed signal
    connect(editor, &QTextEdit::textChanged, this, &EditorWindow::handleTextChanged);
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
}

void EditorWindow::updateTheme() {
    bool isDarkMode = QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    
    QString backgroundColor = isDarkMode ? "#1E1E1E" : "#FFFFFF";
    QString textColor = isDarkMode ? "#D4D4D4" : "#000000";
    QString scrollbarBg = isDarkMode ? "#2D2D2D" : "#F0F0F0";
    QString scrollbarHandle = isDarkMode ? "#4A4A4A" : "#CCCCCC";
    
    editor->setStyleSheet(QString(R"(
        QTextEdit {
            background-color: %1;
            border: none;
            color: %2;
            font-family: Menlo, Monaco, "Courier New", monospace;
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

void EditorWindow::handleTextChanged() {
    if (!unsavedChanges) {
        unsavedChanges = true;
        updateTitle();
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
    if (obj == editor->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        if (wheelEvent->modifiers() & Qt::ControlModifier) {
            const int delta = wheelEvent->angleDelta().y();
            updateZoom(delta > 0 ? 2 : -2);
            return true;
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
