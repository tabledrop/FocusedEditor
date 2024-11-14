#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QString>

class EditorWindow : public QMainWindow {
    Q_OBJECT

public:
    EditorWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void handleTextChanged();
    void saveFile();
    void saveFileAs();
    void openFile();
    void toggleFullscreen();
    void updateTheme();
    void zoomIn();
    void zoomOut();
    void resetZoom();

private:
    void initUI();
    void setupShortcuts();
    bool maybeSave();
    bool saveToFile(const QString& filePath);
    bool loadFile(const QString& filePath);
    void updateTitle();
    void updateZoom(int delta);

    QTextEdit* editor;
    QString currentFile;
    bool unsavedChanges;
    int currentZoom;      // Stores current zoom level (in points)
    const int defaultFontSize = 13;
    const int minFontSize = 8;
    const int maxFontSize = 72;
    const int zoomStep = 1;    // Font size change per zoom step
};
