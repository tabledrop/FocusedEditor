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

private slots:
    void handleTextChanged();
    void saveFile();
    void saveFileAs();
    void openFile();
    void toggleFullscreen();

private:
    void initUI();
    void setupShortcuts();
    bool maybeSave();
    bool saveToFile(const QString& filePath);
    bool loadFile(const QString& filePath);
    void updateTitle();

    QTextEdit* editor;
    QString currentFile;
    bool unsavedChanges;
};
