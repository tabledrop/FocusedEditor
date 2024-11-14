#pragma once

#include <QDialog>
#include <QFont>
#include <QFontComboBox>
#include <QSpinBox>
#include <QTextEdit>

class PreferencesDialog : public QDialog {
    Q_OBJECT

public:
    PreferencesDialog(const QFont& currentFont, QWidget* parent = nullptr);
    QFont getSelectedFont() const;

private slots:
    void previewFont();

private:
    QFontComboBox* fontComboBox;
    QSpinBox* fontSizeSpinner;
    QTextEdit* preview;
};
