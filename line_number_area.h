#pragma once

#include <QWidget>
#include "custom_editor.h"

class LineNumberArea : public QWidget {
    Q_OBJECT

public:
    explicit LineNumberArea(CustomEditor* editor);
    QSize sizeHint() const override;
    void setVisible(bool visible) override;
    bool isVisible() const { return visible; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    CustomEditor* editor;
    bool visible;
    const int horizontalPadding = 5;
    const int minWidth = 30;
};
