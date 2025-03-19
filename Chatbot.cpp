#include "Chatbot.hpp"
#include <iostream>

Chatbot::Chatbot(KaraokeApp& karaokeApp) : app(&karaokeApp), state(0) {
    // Khởi tạo danh sách bài hát từ thư mục E:\C++\MusicBot\Song
    songList = {
        Song("Happy Song", "E:/C++/MusicBot/Song/Wow.mp3"),
        Song("Sad Song", "E:/C++/MusicBot/Song/Loveis.mp3"),
        Song("Bored Song", "E:/C++/MusicBot/Song/Wxrdie.mp3"),
        Song("Motivated Song", "E:/C++/MusicBot/Song/Coldplay.mp3"),
        Song("Tired Song", "E:/C++/MusicBot/Song/Diamond.mp3")
    };
    songList[0].loadLyrics("E:/C++/MusicBot/Song/Wow.lrc");
    songList[1].loadLyrics("E:/C++/MusicBot/Song/Loveis.lrc");
    songList[2].loadLyrics("E:/C++/MusicBot/Song/Wxrdie.lrc");
    songList[3].loadLyrics("E:/C++/MusicBot/Song/Coldplay.lrc");
    songList[4].loadLyrics("E:/C++/MusicBot/Song/Diamond.lrc");
}

void Chatbot::init() {
    if (!font.loadFromFile("C:/Windows/Fonts/segoeui.ttf")) {
        std::cerr << "Không thể mở font Segoe UI.\n";
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cerr << "Không thể mở font Arial.\n";
            exit(1);
        }
    }
    displayText.setFont(font);
    displayText.setCharacterSize(24);
    displayText.setFillColor(sf::Color::White);
    displayText.setPosition(50, 50);
}

void Chatbot::greetUser(sf::RenderWindow& window) {
    displayText.setString(L"Hôm nay bạn thế nào? Tôi sẽ tìm một bản nhạc phù hợp cho bạn nhé!\nNhập 'OK' để tiếp tục.");
    window.draw(displayText);
}

void Chatbot::showEmotionMenu(sf::RenderWindow& window) {
    displayText.setString(
        L"Chọn cảm xúc của bạn:\n"
        L"1. Vui\n"
        L"2. Buồn\n"
        L"3. Chán\n"
        L"4. Cần động lực\n"
        L"5. Mệt mỏi"
    );
    window.draw(displayText);
}

void Chatbot::handleInput(std::wstring input) {
    if (state == 0 && input == L"OK") {
        state = 1; // Chuyển sang hiển thị menu cảm xúc
    }
    else if (state == 1) {
        int choice = std::stoi(input); // Chuyển input thành số
        if (choice >= 1 && choice <= 4) {
            *app = KaraokeApp(songList[choice - 1].title, songList[choice - 1].audioPath, 
                              "E:/C++/MusicBot/Song/" + songList[choice - 1].title + ".lrc");
            app->run(); // Phát nhạc
            state = 2;  // Chuyển sang trạng thái phát nhạc
            displayText.setString(L"Đang phát: " + std::wstring(songList[choice - 1].title.begin(), songList[choice - 1].title.end()));
        }
    }
}

void Chatbot::update(sf::RenderWindow& window) {
    if (state == 0) {
        greetUser(window);
    }
    else if (state == 1) {
        showEmotionMenu(window);
    }
    window.draw(displayText);
}