#include "mainwindow.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTextStream>
#include <QSignalBlocker>
#include <QPushButton>
#include <QDialogButtonBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.pushButton_generate, &QPushButton::clicked, this, &MainWindow::onGenerate);
    connect(ui.pushButton_exportpasswords, &QPushButton::clicked, this, &MainWindow::onExportPasswords);
    connect(ui.pushButton_exportsettings, &QPushButton::clicked, this, &MainWindow::onSaveSettings);
    connect(ui.pushButton_importsettings, &QPushButton::clicked, this, &MainWindow::onLoadSettings);

    connect(ui.checkBox_preventrepeat, &QCheckBox::toggled, this, &MainWindow::onPreventRepeatToggled);

    connect(ui.checkBox_onlylowercase, &QCheckBox::toggled, this, &MainWindow::onCaseBoxToggled);
    connect(ui.checkBox_onlyuppercase, &QCheckBox::toggled, this, &MainWindow::onCaseBoxToggled);

    connect(ui.checkBox_readablepassword, &QCheckBox::toggled, this, &MainWindow::onReadableBoxToggled);
    connect(ui.checkBox_morereadablepassword, &QCheckBox::toggled, this, &MainWindow::onReadableBoxToggled);

    connect(ui.comboBox_preinstalledsets, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onPresetChanged);

    connect(ui.checkBox_lowercase, &QCheckBox::toggled, this, &MainWindow::onCharsetCheckChanged);
    connect(ui.checkBox_uppercase, &QCheckBox::toggled, this, &MainWindow::onCharsetCheckChanged);
    connect(ui.checkBox_digits, &QCheckBox::toggled, this, &MainWindow::onCharsetCheckChanged);
    connect(ui.checkBox_specialchars, &QCheckBox::toggled, this, &MainWindow::onCharsetCheckChanged);

    connect(ui.lineEdit_includedchars, &QLineEdit::textEdited, this, &MainWindow::onIncludeLineEdited);

    onPreventRepeatToggled(ui.checkBox_preventrepeat->isChecked());

    resize(620, 800);
}

PasswordSettings MainWindow::collectSettings() const
{
    PasswordSettings s;

    s.setMinLen(ui.spinBox_min->value());
    s.setMaxLen(ui.spinBox_max->value());
    s.setCount(ui.spinBox_number->value());

    s.setUseLower(ui.checkBox_lowercase->isChecked());
    s.setUseUpper(ui.checkBox_uppercase->isChecked());
    s.setUseDigits(ui.checkBox_digits->isChecked());
    s.setUseSymbols(ui.checkBox_specialchars->isChecked());

    s.setInclude(ui.lineEdit_includedchars->text());
    s.setExclude(ui.lineEdit_excludedchars->text());

    s.setPreventRepeat(ui.checkBox_preventrepeat->isChecked());
    s.setMaxRepeat(ui.spinBox_maxrepeats->value());

    s.setOnlyLower(ui.checkBox_onlylowercase->isChecked());
    s.setOnlyUpper(ui.checkBox_onlyuppercase->isChecked());

    s.setReadable(ui.checkBox_readablepassword->isChecked());
    s.setMoreReadable(ui.checkBox_morereadablepassword->isChecked());

    return s;
}

void MainWindow::applySettings(const PasswordSettings &s)
{
    ui.spinBox_min->setValue(s.minLen());
    ui.spinBox_max->setValue(s.maxLen());
    ui.spinBox_number->setValue(s.count());

    ui.checkBox_lowercase->setChecked(s.useLower());
    ui.checkBox_uppercase->setChecked(s.useUpper());
    ui.checkBox_digits->setChecked(s.useDigits());
    ui.checkBox_specialchars->setChecked(s.useSymbols());

    ui.lineEdit_includedchars->setText(s.includeChars());
    ui.lineEdit_excludedchars->setText(s.excludeChars());

    ui.checkBox_preventrepeat->setChecked(s.preventRepeat());
    ui.spinBox_maxrepeats->setValue(s.maxRepeat());
    onPreventRepeatToggled(s.preventRepeat());

    ui.checkBox_onlylowercase->setChecked(s.onlyLower());
    ui.checkBox_onlyuppercase->setChecked(s.onlyUpper());
    ui.checkBox_readablepassword->setChecked(s.readable());
    ui.checkBox_morereadablepassword->setChecked(s.moreReadable());

    ui.comboBox_preinstalledsets->setCurrentIndex(0);
}

void MainWindow::onGenerate()
{
    if (ui.spinBox_min->value() <= 0 ||
        ui.spinBox_max->value() <= 0 ||
        ui.spinBox_number->value() <= 0)
    {
        QMessageBox::warning(this, "Ошибка",
                             "Минимальная длина, максимальная длина и количество "
                             "паролей должны быть больше нуля.");
        return;
    }

    if (ui.spinBox_max->value() < ui.spinBox_min->value())
    {
        QMessageBox::warning(this, "Ошибка",
                             "Максимальная длина пароля не может быть меньше минимальной.");
        return;
    }

    PasswordSettings s = collectSettings();

    gen.setSettings(s);
    QStringList list = gen.generate();

    for (const QString &p : list)
    {
        if (p.startsWith("Ошибка"))
        {
            QString msg = p;
            const QString prefix = QStringLiteral("Ошибка: ");
            if (msg.startsWith(prefix))
                msg.remove(0, prefix.size()).trimmed();

            QMessageBox::warning(this, "Ошибка", msg);
            return;
        }
    }

    ui.textEdit_result->clear();
    for (const QString &p : list)
        ui.textEdit_result->append(p);
}

void MainWindow::onExportPasswords()
{
    QString txt = ui.textEdit_result->toPlainText();
    if (txt.trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Нет данных для экспорта");
        return;
    }

    QString file = QFileDialog::getSaveFileName(this, "Сохранить пароли", "", "Текстовый документ (*.txt)");
    if (file.isEmpty()) return;

    QFile f(file);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Ошибка", "Не удаётся открыть файл");
        return;
    }
    QTextStream(&f) << txt;
    f.close();
    QMessageBox::information(this, "Экспорт", "Пароли сохранены");
}

void MainWindow::onSaveSettings()
{
    QString file = QFileDialog::getSaveFileName(this, "Сохранить настройки", "", "Файл JSON (*.json)");
    if (file.isEmpty()) return;

    QFile f(file);
    if (!f.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, "Ошибка", "Не удаётся открыть файл");
        return;
    }

    QJsonObject obj = collectSettings().toJson();
    obj["presetText"] = ui.comboBox_preinstalledsets->currentText();

    QJsonDocument doc(obj);
    f.write(doc.toJson());
    f.close();
}

void MainWindow::onLoadSettings()
{
    QString file = QFileDialog::getOpenFileName(this, "Загрузить настройки", "", "Файл JSON (*.json)");
    if (file.isEmpty()) return;

    QFile f(file);
    if (!f.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Ошибка", "Не удаётся открыть файл");
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    if (!doc.isObject())
    {
        QMessageBox::critical(this, "Ошибка", "Неверный формат JSON");
        return;
    }

    QJsonObject obj = doc.object();

    PasswordSettings s; s.fromJson(obj);
    applySettings(s);

    const QString presetText = obj.value("presetText").toString();
    if (!presetText.isEmpty())
    {
        int idx = ui.comboBox_preinstalledsets->findText(presetText);
        if (idx >= 0)
        {
            QSignalBlocker blocker(ui.comboBox_preinstalledsets);
            ui.comboBox_preinstalledsets->setCurrentIndex(idx);
        }
    }
}

void MainWindow::onPreventRepeatToggled(bool on)
{
    ui.spinBox_maxrepeats->setEnabled(on);

    if (!on)
        ui.spinBox_maxrepeats->setValue(0);
}

void MainWindow::onCaseBoxToggled(bool checked)
{
    QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
    if (!checked || !cb) return;

    if (cb == ui.checkBox_onlylowercase)
        ui.checkBox_onlyuppercase->setChecked(false);
    else
        ui.checkBox_onlylowercase->setChecked(false);
}

void MainWindow::onReadableBoxToggled(bool checked)
{
    QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
    if (!checked || !cb) return;

    if (cb == ui.checkBox_readablepassword)
        ui.checkBox_morereadablepassword->setChecked(false);
    else
        ui.checkBox_readablepassword->setChecked(false);
}

void MainWindow::onPresetChanged(int idx)
{
    if (idx == 0) return;

    bool lower = true, upper = true, digits = false, symbols = false;
    if (idx == 2) digits = true;
    if (idx == 3) { digits = true; symbols = true; }

    {
        QSignalBlocker b(ui.checkBox_lowercase);
        ui.checkBox_lowercase->setChecked(lower);
    }
    {
        QSignalBlocker b(ui.checkBox_uppercase);
        ui.checkBox_uppercase->setChecked(upper);
    }
    {
        QSignalBlocker b(ui.checkBox_digits);
        ui.checkBox_digits->setChecked(digits);
    }
    {
        QSignalBlocker b(ui.checkBox_specialchars);
        ui.checkBox_specialchars->setChecked(symbols);
    }

    updateIncludeLine();
}

void MainWindow::onCharsetCheckChanged(bool)
{
    ui.comboBox_preinstalledsets->setCurrentIndex(0);
    updateIncludeLine();
}

void MainWindow::updateIncludeLine()
{
    QString s;
    if (ui.checkBox_lowercase->isChecked()) s += "abcdefghijklmnopqrstuvwxyz";
    if (ui.checkBox_uppercase->isChecked()) s += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (ui.checkBox_digits->isChecked()) s += "0123456789";
    if (ui.checkBox_specialchars->isChecked())s += "!@#$%^&*()_+-=[]{}|;:,.<>?";

    ui.lineEdit_includedchars->setText(s);
}

void MainWindow::onIncludeLineEdited(const QString &)
{
    if (ui.comboBox_preinstalledsets->currentIndex() != 0)
        ui.comboBox_preinstalledsets->setCurrentIndex(0);
}
