#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "transcribemodule.h"
#include "llamamodule.h"
#include "audionormalizer.h"
#include "utils.h"
#include <QInputDialog>
#include <iostream>
#include <fstream>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QUuid>
#include <QDir>
#include <Windows.h>
#include <chrono>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(TranscribeModule &transcriber, llamaModule &llama,  AudioNormalizer &normalizer, QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_promptButton_clicked();
    void on_queueButton_clicked();
    void on_promptsSelect_currentIndexChanged(int index);
    void on_saveButton_clicked();
    void refresh_prompts();

    void on_deleteButton_clicked();

private:
    Ui::MainWindow *ui;
    TranscribeModule &transcriber;
    llamaModule &llama;
    AudioNormalizer &normalizer;
    std::map<std::string, std::string> &prompts;
};
#endif // MAINWINDOW_H
