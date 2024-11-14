#pragma once

#include <QMainWindow>
#include "custom_editor.h"
#include <QString>
#include "code_highlighter.h"
#include "indent_manager.h"
#include "line_number_area.h"

class EditorWindow : public QMainWindow {
    Q_OBJECT

public:
    EditorWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void handleTextChanged();
    bool saveFile();
    void saveFileAs();
    void openFile();
    void toggleFullscreen();
    void updateTheme();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void showPreferences();
    void toggleLineNumbers();
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect& rect, int dy);

private:
    void initUI();
    void setupShortcuts();
    bool maybeSave();
    bool saveToFile(const QString& filePath);
    void loadFile(const QString& filePath);
    void updateTitle();
    void updateZoom(int delta);
    void showSplashScreen();
    void hideSplashScreen();
    void updateSyntaxHighlighting();

    CustomEditor* editor;
    QString currentFile;
    bool unsavedChanges;
    int currentZoom;
    const int defaultFontSize = 13;
    const int minFontSize = 8;
    const int maxFontSize = 72;
    const int zoomStep = 1;
    bool showingSplash;
    CodeHighlighter* highlighter;
    IndentManager* indentManager;
    LineNumberArea* lineNumberArea;
};
