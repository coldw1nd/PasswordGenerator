#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "passwordgenerator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onGenerate();
    void onExportPasswords();
    void onSaveSettings();
    void onLoadSettings();

    void onPreventRepeatToggled(bool);
    void onCaseBoxToggled(bool);
    void onReadableBoxToggled(bool);
    void onPresetChanged(int);
    void onCharsetCheckChanged(bool);

    void onIncludeLineEdited(const QString&);

private:
    PasswordSettings collectSettings() const;
    void applySettings(const PasswordSettings&);

    void updateIncludeLine();

    Ui::MainWindow ui;
    PasswordGenerator gen;
};

#endif // MAINWINDOW_H
