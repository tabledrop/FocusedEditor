#include "line_number_area.h"
#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QDebug>

LineNumberArea::LineNumberArea(CustomEditor* editor)
    : QWidget(editor)
    , editor(editor)
    , visible(true)
{
    setVisible(true);
}

QSize LineNumberArea::sizeHint() const {
    // Calculate width based on the number of digits in the last line number
    int digits = 1;
    int max = qMax(1, editor->document()->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    
    // Calculate width based on font metrics
    QFontMetrics metrics(editor->font());
    int spaceWidth = metrics.horizontalAdvance(QLatin1Char('9'));
    int width = spaceWidth * digits + 2 * horizontalPadding;
    width = qMax(width, minWidth);
    
    return QSize(width, 0);
}

void LineNumberArea::setVisible(bool visible) {
    if (this->visible == visible) return;
    
    this->visible = visible;
    QWidget::setVisible(visible);
    
    // Update editor margins
    if (visible) {
        editor->setCustomViewportMargins(sizeHint().width(), 0, 0, 0);
    } else {
        editor->setCustomViewportMargins(0, 0, 0, 0);
    }
}

void LineNumberArea::paintEvent(QPaintEvent* event) {
    if (!visible) return;
    
    QPainter painter(this);
    
    // Match editor colors for consistency
    bool isDarkMode = editor->palette().color(QPalette::Base).lightness() < 128;
    QColor bgColor = isDarkMode ? QColor("#1E1E1E") : QColor("#F0F0F0");
    QColor textColor = isDarkMode ? QColor("#6D6D6D") : QColor("#999999");
    
    // Fill background
    painter.fillRect(event->rect(), bgColor);
    
    // Get viewport offset
    int viewportOffset = editor->verticalScrollBar()->value();
    
    // Get first visible block and its geometry
    QTextBlock block = editor->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    QRectF blockGeometry = editor->blockBoundingGeometry(block);
    
    // Set font and metrics
    painter.setFont(editor->font());
    QFontMetrics metrics(editor->font());
    
    // Calculate initial position
    qreal top = blockGeometry.translated(editor->contentOffset()).top();
    
    // Paint line numbers
    while (block.isValid()) {
        if (block.isVisible()) {
            QString number = QString::number(blockNumber + 1);
            
            // Get block dimensions
            QRectF blockRect = editor->blockBoundingRect(block);
            bool isWrapped = blockRect.height() > metrics.height();
            
            // For wrapped lines, only use the height of one line
            qreal drawHeight = isWrapped ? metrics.height() : blockRect.height();
            
            // Draw the line number
            painter.setPen(textColor);
            painter.drawText(
                horizontalPadding,
                top,
                width() - 2 * horizontalPadding,
                drawHeight,
                Qt::AlignRight | Qt::AlignVCenter,
                number
            );
            
            top += blockRect.height();
        }
        
        block = block.next();
        ++blockNumber;
    }
}
