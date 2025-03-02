#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(TranscribeModule &transcriber, llamaModule &llama, AudioNormalizer &normalizer,QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), transcriber(transcriber), llama(llama), normalizer(normalizer),
    prompts(Utils::parse(Utils::save_path))
{
    ui->setupUi(this);
    ui->languageBox->addItem("it");
    ui->languageBox->addItem("en");
    refresh_prompts();
}

MainWindow::~MainWindow()
{
    Utils::save_file(prompts);
    delete ui;
}


void MainWindow::on_promptButton_clicked()
{
    std::string prompt = ui->promptBox->toPlainText().toStdString();
    std::string response="";
    ui->responseBox->clear();
    size_t chunkSize = ui->inputLetters->text().toUInt();
    assert(chunkSize>0);
    for(size_t i=0; i< prompt.size(); i+=chunkSize){
        response="";
        while (Utils::isBlank(response)){
            response = llama.getAnswer(prompt);
            if(response.length()==0){
                llama.resetContext(DEFAULT_CTX_SIZE);
            }
            std::cout<<"I have created a response "<<response.length()<<" letters long"<<std::endl;
            ui->responseBox->append(QString::fromStdString(response)+"\n");
        }
    }

}


void MainWindow::on_queueButton_clicked()
{
    QStringList audioFiles = QFileDialog::getOpenFileNames(
        this,
        tr("Select Audio Files"),
        QString(),
        tr("Audio Files (*.mp3 *.wav *.flac);;All Files (*)")
        );

    QString saveFolder = QFileDialog::getExistingDirectory();
    std::cout<<saveFolder.toStdString()<<std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (const QString &file : audioFiles) {
        llama.resetContext(DEFAULT_CTX_SIZE);
        std::cout<<file.toStdString()<<std::endl;
        QFileInfo fileInfo(file);
        QString notesFile = QDir(saveFolder).filePath("notes_"+fileInfo.completeBaseName()+".txt");
        QString transFile = QDir(saveFolder).filePath("transcript_"+fileInfo.completeBaseName()+".txt");
        std::cout<<notesFile.toStdString()<<" "<<transFile.toStdString()<<std::endl;

        QString uniqueFileName = QString(".tmp_%1.wav").arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
        std::string tmp = QDir(fileInfo.absolutePath()).filePath(uniqueFileName).toStdString();

        normalizer.transcode_audio(file.toStdString().c_str(), tmp.c_str());

        SetFileAttributesA(tmp.c_str(), FILE_ATTRIBUTE_HIDDEN);

        std::cout << "Generated hidden file: " << tmp << std::endl;
        std::string language =  ui->languageBox->currentText().toStdString();
        std::cout<<"The current language is: "<< language <<std::endl;
        std::string text = transcriber.transcribeText(tmp, language);
        std::ofstream outFile2(transFile.toStdString());
        if (!outFile2) {
            std::cerr << "Error opening file for writing\n";
        }
        outFile2 << text;
        outFile2.close();
        std::string prompt = ui->promptBox->toPlainText().toStdString();

        std::ofstream outFile(notesFile.toStdString());
        if (!outFile) {
            std::cerr << "Error opening file for writing\n";
        }

        std::string response = "";
        size_t chunkSize = ui->inputLetters->text().toUInt();
        assert(chunkSize>0);
        int j=1;
        for(size_t i=0; i< text.size(); i+=chunkSize){
            std::string tmp = prompt;
            std::string input = tmp.append(text.substr(i, chunkSize));
            response="";
            while (Utils::isBlank(response)){
                response = llama.getAnswer(input);
                if(response.length()==0){
                    llama.resetContext(DEFAULT_CTX_SIZE);
                }
                std::cout<<"I have created a response "<<response.length()<<" letters long"<<std::endl;
            }
            outFile<< "Part "<<j<<std::endl;
            outFile << response <<std::endl;
            ++j;
        }
        outFile.close();

        QFile tempFile(QString::fromStdString(tmp));
        if (tempFile.exists()) {
            if (tempFile.remove()) {
                std::cout << "Temporary file deleted successfully: " << tmp << std::endl;
            } else {
                std::cerr << "Failed to delete the temporary file: " << tmp << std::endl;
            }
        }


    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time spent: " << duration.count() << " ms" << std::endl;

}



void MainWindow::on_promptsSelect_currentIndexChanged(int index)
{
    auto it = prompts.find(ui->promptsSelect->currentText().toStdString());

    if (it != prompts.end()) {
        ui->promptBox->setText(QString::fromStdString(it->second));
    } else {
        std::cout << "Key not found!" << std::endl;
    }
}




void MainWindow::on_saveButton_clicked()
{
    std::string text = ui->promptBox->toPlainText().toStdString();
    std::string userInput = QInputDialog::getText(nullptr, "Input", "Enter the name of the prompt: ").toStdString();
    std::cout<<userInput<<std::endl;
    prompts[userInput] = text;
    refresh_prompts();
}

void MainWindow::refresh_prompts(){
    ui->promptsSelect->clear();
    for (const auto& p : prompts) {
        ui->promptsSelect->addItem(QString::fromStdString(p.first));
    }
}


void MainWindow::on_deleteButton_clicked()
{
    auto it = prompts.find(ui->promptsSelect->currentText().toStdString());


    if (it != prompts.end()) {
        prompts.erase(it);
        refresh_prompts();
    } else {
        std::cout << "Key not found!" << std::endl;
    }
    ui->promptBox->clear();
}

