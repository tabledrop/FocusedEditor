#include "preferences_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>

PreferencesDialog::PreferencesDialog(const QFont& font, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Preferences"));
    
    auto layout = new QVBoxLayout(this);
    
    // Font selection section
    auto fontGroup = new QWidget(this);
    auto fontLayout = new QHBoxLayout(fontGroup);
    
    fontLayout->addWidget(new QLabel(tr("Font:")));
    
    fontComboBox = new QFontComboBox;
    fontComboBox->setFontFilters(QFontComboBox::MonospacedFonts);
    fontComboBox->setCurrentFont(font);
    fontLayout->addWidget(fontComboBox);
    
    fontLayout->addWidget(new QLabel(tr("Size:")));
    
    fontSizeSpinner = new QSpinBox;
    fontSizeSpinner->setRange(8, 72);
    fontSizeSpinner->setValue(font.pointSize());
    fontLayout->addWidget(fontSizeSpinner);
    
    layout->addWidget(fontGroup);
    
    // Preview section
    auto previewLabel = new QLabel(tr("Preview:"));
    layout->addWidget(previewLabel);
    
    preview = new QTextEdit;
    preview->setPlainText("ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
                         "abcdefghijklmnopqrstuvwxyz\n"
                         "1234567890 !@#$%^&*()");
    preview->setReadOnly(true);
    preview->setMaximumHeight(100);
    preview->setFont(font);
    layout->addWidget(preview);
    
    // Buttons
    auto buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    // Connect preview updates
    connect(fontComboBox, &QFontComboBox::currentFontChanged,
            this, &PreferencesDialog::previewFont);
    connect(fontSizeSpinner, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PreferencesDialog::previewFont);
    
    resize(500, 300);
}

void PreferencesDialog::previewFont()
{
    QFont font = fontComboBox->currentFont();
    font.setPointSize(fontSizeSpinner->value());
    preview->setFont(font);
}

QFont PreferencesDialog::getSelectedFont() const
{
    QFont font = fontComboBox->currentFont();
    font.setPointSize(fontSizeSpinner->value());
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    return font;
}
