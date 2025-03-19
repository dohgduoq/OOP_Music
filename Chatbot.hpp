#ifndef CHATBOT_HPP
#define CHATBOT_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "main.cpp" // Để truy cập các class Song, MusicPlayer, KaraokeApp

class Chatbot {
private:
    sf::Font font;
    sf::Text displayText;
    std::vector<Song> songList; // Danh sách bài hát
    KaraokeApp* app;            // Con trỏ đến KaraokeApp để chạy nhạc
    int state;                  // Trạng thái chatbot (0: chào, 1: chờ OK, 2: chọn cảm xúc, 3: phát nhạc)
    std::wstring userInput;     // Lưu input từ người dùng (wstring để hỗ trợ tiếng Việt)

public:
    Chatbot(KaraokeApp& karaokeApp);
    void init();                // Khởi tạo font và text
    void greetUser(sf::RenderWindow& window); // Hiển thị lời chào
    void showEmotionMenu(sf::RenderWindow& window); // Hiển thị menu cảm xúc
    void handleInput(std::wstring input); // Xử lý input từ người dùng
    void update(sf::RenderWindow& window); // Cập nhật giao diện chatbot
};

#endif