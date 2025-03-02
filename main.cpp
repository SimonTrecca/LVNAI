#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <QTimer>
#include "transcribemodule.h"
#include "llamamodule.h"
#include "audionormalizer.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TranscribeModule transcriber;
    llamaModule llama;
    AudioNormalizer normalizer;

    MainWindow w(transcriber, llama, normalizer);


    w.show();
    return a.exec();
}

