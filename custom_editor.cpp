#include "custom_editor.h"

CustomEditor::CustomEditor(QWidget* parent)
    : QPlainTextEdit(parent)
{
}

void CustomEditor::setCustomViewportMargins(int left, int top, int right, int bottom) {
    setViewportMargins(left, top, right, bottom);
}
