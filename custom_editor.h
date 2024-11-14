#pragma once

#include <QPlainTextEdit>

class LineNumberArea;  // Forward declaration

class CustomEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit CustomEditor(QWidget* parent = nullptr);
    void setCustomViewportMargins(int left, int top, int right, int bottom);

    // Make these methods available to LineNumberArea
    friend class LineNumberArea;
    using QPlainTextEdit::blockBoundingGeometry;
    using QPlainTextEdit::contentOffset;
    using QPlainTextEdit::firstVisibleBlock;
};
