#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <filesystem>
#include <windows.h>

using namespace std;

// Class Song: Lưu thông tin của một bài hát
class Song {
public:
    string title;
    string artist;
    string album;
    string audioPath;
    string lyricPath;
    string imagePath; // Đường dẫn đến ảnh bìa
    vector<pair<int, wstring>> lyrics;

    Song(string t, string art, string alb, string img, string path) 
        : title(t), artist(art), album(alb), imagePath(img), audioPath(path) {
        lyricPath = path.substr(0, path.find_last_of(".")) + ".lrc";
    }

    void loadLyrics() {
        wifstream file(lyricPath, ios::binary);
        file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));

        if (!file) {
            cerr << "Không thể mở file lyric: " << lyricPath << endl;
            return;
        }

        wstring line;
        while (getline(file, line)) {
            if (line.empty() || line[0] != L'[') continue;

            int minutes, seconds, milliseconds = 0;
            wstring lyricText;
            size_t pos = line.find(L"]");
            if (pos == wstring::npos) continue;

            wstring timeStr = line.substr(1, pos - 1);
            lyricText = line.substr(pos + 1);

            if (swscanf(timeStr.c_str(), L"%d:%d.%d", &minutes, &seconds, &milliseconds) >= 2) {
                int totalMilliseconds = (minutes * 60 + seconds) * 1000 + milliseconds * 10;
                lyrics.push_back({totalMilliseconds, lyricText});
            }
        }
        file.close();
    }

    wstring getLyricAtTime(int currentTimeMs) {
        if (lyrics.empty()) return L"";
        for (size_t i = 0; i < lyrics.size(); i++) {
            if (currentTimeMs < lyrics[i].first) {
                return (i > 0) ? lyrics[i - 1].second : L"";
            }
        }
        return lyrics.back().second;
    }
};

// Class MusicPlayer: Phát nhạc từ file MP3
class MusicPlayer {
public:
    sf::Music music;
    Song* currentSong;

    MusicPlayer() : currentSong(nullptr) {}

    void playSong(Song& song) {
        currentSong = &song;
        if (music.openFromFile(song.audioPath)) {
            music.play();
        }
        else {
            cerr << "Không thể mở file MP3: " << song.audioPath << endl;
        }
    }

    void pause() { music.pause(); }
    void stop() { music.stop(); }
    int getCurrentTimeMs() { return music.getPlayingOffset().asMilliseconds(); }
    bool isPlaying() { return music.getStatus() == sf::Music::Playing; }
};

// Class KaraokeApp: Hiển thị lời bài hát
class KaraokeApp {
public:
    MusicPlayer player;
    Song song;
    sf::Font font;

    KaraokeApp(string title, string art, string alb, string img, string audioPath)
        : song(title, art, alb, img, audioPath) {
        song.loadLyrics();
    }

    // Khởi tạo font chữ cho lời bài hát tiếng Việt
    void initFont() {
        if (!font.loadFromFile("C:/Windows/Fonts/segoeui.ttf")) {
            cerr << "Khong the mo font Segoe UI. Chuyen sang Arial.\n";
            if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
                cerr << "Khong the mo font Arial.\n";
                exit(1);
            }
        }
    }

    // Hiển thị lời bài hát và phát nhạc trên cửa sổ
    void run() {
        sf::RenderWindow window(sf::VideoMode(800, 600), "Music Player", sf::Style::Close);
        window.setFramerateLimit(60);
        initFont();

        // Tạo nền
        sf::RectangleShape background(sf::Vector2f(800, 600));
        background.setFillColor(sf::Color(26, 26, 26));

        // Phần lyric
        sf::RectangleShape lyricBox(sf::Vector2f(800, 300));
        lyricBox.setPosition(0, 300);
        lyricBox.setFillColor(sf::Color(40, 40, 40));

        // Hiển thị lyric
        sf::Text lyricText;
        lyricText.setFont(font);
        lyricText.setCharacterSize(30);
        lyricText.setFillColor(sf::Color(169, 169, 169));
        lyricText.setPosition(10, 380);

        // Ảnh bài hát
        sf::Texture albumTexture;
        if (!albumTexture.loadFromFile(song.imagePath)) {
            std::cerr << "Failed to load image '" << song.imagePath << "'" << std::endl;
            return;
        }
        sf::CircleShape albumArt(150);
        albumArt.setTexture(&albumTexture);
        albumArt.setPosition(150, 150);
        albumArt.setOrigin(150, 150);

        // Thông tin bài hát
        sf::Text songTitle(song.title, font, 40);
        songTitle.setFillColor(sf::Color::White);
        songTitle.setPosition(350, 100);

        sf::Text artist(song.artist, font, 20);
        artist.setFillColor(sf::Color(169, 169, 169));
        artist.setPosition(350, 170);

        sf::Text album(song.album, font, 20);
        album.setFillColor(sf::Color(169, 169, 169));
        album.setPosition(350, 200);

        // Phát nhạc
        player.playSong(song);
        player.music.setLoop(true);

        while (window.isOpen() && player.isPlaying()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            // Cập nhật lyric theo thời gian
            wstring lyricW = song.getLyricAtTime(player.getCurrentTimeMs());
            // string lyricStr(lyricW.begin(), lyricW.end()); // Chuyển wstring sang string
            lyricText.setString(lyricW);

            // Xoay đĩa
            // static sf::Clock rotationClock;
            // float deltaTime = rotationClock.restart().asSeconds();
            // float rotationSpeed = 60.0f; // Góc xoay mỗi giây (độ)
            // albumArt.rotate(rotationSpeed * deltaTime);
            albumArt.rotate(1.5f);
            window.clear();

            // Vẽ giao diện
            window.draw(background);
            window.draw(lyricBox);
            window.draw(albumArt);
            window.draw(songTitle);
            window.draw(artist);
            window.draw(album);
            window.draw(lyricText);

            window.display();

            this_thread::sleep_for(chrono::milliseconds(100));
        }
        player.stop();
    }
};

// Class Chatbot: Giao diện dòng lệnh để chọn bài hát
class Chatbot {
private:
    vector<Song> songList;
    KaraokeApp* app;
    sf::Font font;
    sf::Text displayText;

public:
    // Khởi tạo danh sách bài hát với thông tin riêng
    Chatbot() : app(nullptr) {
        songList = {
            Song("Wow", "Post Malone", "Hollywood's Bleeding", "E:/C++/MusicBot/Song/Wow.jpg", "E:/C++/MusicBot/Song/Wow.mp3"),
            Song("Love is", "Dangrangto", "Workaholics", "E:/C++/MusicBot/Song/Loveis.jpg", "E:/C++/MusicBot/Song/Loveis.mp3"),
            Song("NEP VAO SAT VAO", "Wxrdie", "Nick Schmidt", "E:/C++/MusicBot/Song/Wxrdie.jpg", "E:/C++/MusicBot/Song/Wxrdie.mp3"),
            Song("Viva la Vida", "Coldplay", "Viva la Vida", "E:/C++/MusicBot/Song/Coldplay.jpg", "E:/C++/MusicBot/Song/Coldplay.mp3"),
            Song("Diamonds", "Rihanna", "ANTI", "E:/C++/MusicBot/Song/Diamond.jpg", "E:/C++/MusicBot/Song/Diamond.mp3")
        };
    }

    // Khởi tạo font chữ
    void init() {
        if (!font.loadFromFile("C:/Windows/Fonts/segoeui.ttf")) {
            cerr << "Khong the mo font Segoe UI. Chuyen sang Arial.\n";
            if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
                cerr << "Khong the mo font Arial.\n";
                exit(1);
            }
        }
        displayText.setFont(font);
        displayText.setCharacterSize(24);
        displayText.setFillColor(sf::Color::White);
        displayText.setPosition(50, 50);
    }

    // Chạy chatbot để chọn bài hát
    void runChatbot() {
        wcout << L"Chatbot: Hom nay cua ban the nao? Toi se tim mot ban nhac phu hop cho ban!\nNhap OK de tiep tuc: ";
        wstring userInput;
        wcin >> userInput;

        if (userInput != L"ok" && userInput != L"OK") {
            wcout << L"Chatbot: Ban can nhap 'OK' hoac 'ok' de tiep tuc!\n";
            return;
        }

        wcout << L"Chon cam xuc hien tai cua ban di:3 :\n"
            "1. Vui\n"
            "2. Buon\n"
            "3. Chan\n"
            "4. Can them dong luc\n"
            "5. Met moi\n"
            "Nhap so thay cho lua chon nhe ._. : ";

        int choice;
        wcin >> choice;

        if (choice < 1 || choice > 5) {
            wcout << L"Chatbot: Lua chon chua phu hop roai!\n";
            return;
        }

        wcout << L"Chatbot: Loading... " << wstring(songList[choice - 1].title.begin(), songList[choice - 1].title.end()) << L"\n";

        app = new KaraokeApp(songList[choice - 1].title, songList[choice - 1].artist, 
                            songList[choice - 1].album, songList[choice - 1].imagePath, 
                            songList[choice - 1].audioPath);
        app->run();
        delete app;
    }
};

// Hàm main
int main() {
    Chatbot chatbot;
    chatbot.init();
    chatbot.runChatbot();
    return 0;
}

// Hàm WinMain để chạy ứng dụng Windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main();
}