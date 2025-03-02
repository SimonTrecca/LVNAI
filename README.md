# LVNAI
Converts speech to text and processes it using local AI models.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Models](#models)

## Introduction
LVNAI is a voice-powered note-taking application designed to automatically create structured and proper notes from audio recordings. By running inference on a local large language model (LLM), LVNAI transforms raw audio into concise and well-organized notes. This is by no measure state of the art, and it's just a fun project to help me in note-taking.

## Features
- **Automatic Note Creation:** Converts audio recordings into structured text notes.
- **Speech-to-Text Conversion:** Uses [whisper.cpp](https://github.com/ggerganov/whisper.cpp) for converting speech to text.
- **Audio Formatting:** Utilizes ffmpeg libraries to properly format audio files for the whisper model.
- **Efficient Inference:** Splits text output into manageable segments before processing with [llama.cpp](https://github.com/ggerganov/llama.cpp) to preserve context and reduce performance overhead.
- **Graphical User Interface:** A fully functional GUI built with Qt, providing a user-friendly way to interact with the tool.
- **Prompt Management:** Save custom prompts for reuse on different audio files.
- **Batch Processing:** Queue multiple audio files for processing simultaneously.

## Installation
1. **Prerequisites:**
   - Ensure you have a C/C++ compiler installed.
   - Install [ffmpeg](https://ffmpeg.org/).
   - Install Qt (for the GUI) and its development libraries.
   - Download and build the [whisper.cpp](https://github.com/ggerganov/whisper.cpp) and [llama.cpp](https://github.com/ggerganov/llama.cpp) libraries.
2. **Clone the Repository:**
   ```bash
   git clone https://github.com/SimonTrecca/LVNAI.git
   cd LVNAI
3. **Install the dependencies:**
- It should look something like this:
  
![image](https://github.com/user-attachments/assets/68d9b524-f2fa-4fdd-9be2-664c7b87ee1b)
![image](https://github.com/user-attachments/assets/b3e02f4f-7cea-4968-807c-48e60b3fb254)

4. **Compile the project and set up the build folder with the files needed:**
- The folder should look something like this, with the folders containing your favorite model of choice. The models need to be named model.bin (for whisper.cpp) and model.gguf (for llama.cpp)
  
  ![image](https://github.com/user-attachments/assets/57b0c6a9-fb60-4a61-b01d-9c04d8f3fb2e)

  I also provide an already compiled version using Vulkan GPU acceleration for both whisper.cpp and llama.cpp
## Models
You can use whatever models you want for both whisper.cpp and llama.cpp; it is recommended to use models that are approximately 1-2 GB smaller than your total GPU VRAM at max, or at least less than the sum of your GPU VRAM and system RAM, to ensure optimal performance.
You can find all models here: 
https://huggingface.co/models

