#pragma once
#ifndef CHATWINDOW_H
#define CHATWINDOW_H
#include "NetWork.h"
#include <string>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_5.h>
#include <tchar.h>
#include <vector>
#include <sstream>

#include <fmod.hpp>
#include <fmod_errors.h>
#include <cmath>
#include <thread>
#include <chrono>
#include <conio.h>
#pragma comment(lib, "fmod_vc.lib")

//create a chat window, fixed size, no resize, no move, no title bar
class MessageBlock {
private:
    int num;
    std::string content;
    std::string username;//name must be unique and longer than 2 chars
    ImVec2 position;
	float height;
    bool isUser = false;
    char name2[3] = { "\0" };
    float block_width;
    ImVec2 space;
    std::string target;
    //int heightPos = 0;
public:
    MessageBlock();
    ~MessageBlock() {};
	//set message block content and id
    void setBlock(int _num, std::string _content, std::string _username, ImVec2 _position, bool _isUser, const float main_scale, std::string _target = "all");
	//draw the message block
	void drawBlock(const ImGuiWindowFlags& child_flags, bool needIcon = true);

    void drawIcon();

    float getHeight() {
        if (height == 0) {
            float wrap_w = block_width - space.x * 2.0f;//*2 for left and right
            ImVec2 text_sz = ImGui::CalcTextSize(content.c_str(), nullptr, false, wrap_w);
            height = space.y * 2.0f + text_sz.y;
        }
        return height; 
    }

    const std::string getUserName() { return username; }

    const std::string getTarget() { return target; }

    const int getBlockHeightPos() { return position.y; }

    void setBlockHeightPos(int posY) { position.y = posY; }

    
};
class OnlineUsers {
public:
    std::vector<std::string> names;
    OnlineUsers(std::string userName) { addUser(userName); }
    void addUser(const std::string& name) {
        names.push_back(name);
    }
    void eraseUser(const std::string& name) {

        auto it = std::find(names.begin(), names.end(), name);
        if (it != names.end()) {
            names.erase(it);
        }
        else {
            std::cout << "user doesnt exist!" << std::endl;
        }

    }
    void getUserList(std::string msg) {
        std::stringstream ss(msg);
        std::string item;
        while (std::getline(ss, item, '#')) {
            if (!item.empty()) {
                names.push_back(item);
                std::cout << item << std::endl;
            }
        }
    }
};
class ChatWindow {
public:
    bool nameInvalid = false;
    bool hasChildWindow = false;
    std::string dmTarget = "Unknown";
    ImGuiWindowFlags window_flags = 0;
    ImGuiWindowFlags child_flags = 0;
    std::vector<MessageBlock> messageBlocks;
    float currentHeight = 0;
    std::string username = "Unknown";
    ChatWindow();
    ~ChatWindow() { messageBlocks.clear(); };
	//add a message to the chat window
    void addMessage(const std::string& content, const std::string& username, bool isUser, const float main_scale = 1.f, std::string target = "all");
	//draw the chat window
    void render(Client& client, bool& hasName);

    void setUsername(std::string _username) { 
        username = _username; 
    }
    void openDmWindow(const std::string& name) {
        dmTarget = name;
        hasChildWindow = true;
        std::cout << "open dm window" << std::endl;
    }
    void closeDmWindow() {
        dmTarget = "Unknown";
        hasChildWindow = false;
        std::cout << "close dm window" << std::endl;
    }
    void drawIcon(const std::string& name) {
        static ImU32 userColor = IM_COL32(215, 215, 215, 255);
        static ImU32 normalColor = IM_COL32(70, 70, 70, 255);
        static ImU32 activeColor = IM_COL32(23, 77, 227, 255);
        static ImU32 hoveredColor = IM_COL32(168, 226, 237, 255);
        static ImU32 borderColor = IM_COL32(255, 255, 255, 255);

        const static ImVec4 iconText = ImVec4(0.05882f * 0.6, 0.20000f * 0.6, 0.36863f * 0.6, 1.f);//39, 56, 30
        ImVec2 size(35, 35);
        bool clicked = false;
        std::string id = name;//"##" can make a unique id
        if (ImGui::InvisibleButton(id.c_str(), size)) {
            clicked = !clicked;
        }
        bool hovered = ImGui::IsItemHovered();

        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 p0 = ImGui::GetItemRectMin();
        ImVec2 p1 = ImGui::GetItemRectMax();

        ImU32 fill;
        if (!hovered) {
            if (name == dmTarget)
                fill = activeColor;
            else if (name == username)
                fill = userColor;
            else
                fill = normalColor;
        }
        else {
            fill = hoveredColor;
        }

        if (clicked) {
            fill = IM_COL32(255, 255, 255, 255);
            std::cout << "Clicked on user: " << name << std::endl;
            //we can do DM function here, for example, open a new chat window with this user
            if (name == dmTarget || name == username) {
                closeDmWindow();
            }
            else {
                openDmWindow(name);
            }
        }

        dl->AddRectFilled(p0, p1, fill, 4.0f);
        dl->AddRect(p0, p1, hoveredColor, 4.0f, 0, 1.0f);

        ImGui::SetItemTooltip(name.c_str());

        //add name
        char name2[3];
        name2[0] = name[0]; name2[1] = name[1]; name2[2] = '\0';
        const char* text = name2;
        ImVec2 text_size = ImGui::CalcTextSize(text);
        ImVec2 center = ImVec2(
            p0.x + (size.x - text_size.x) * 0.5f,
            p0.y + (size.y - text_size.y) * 0.5f
        );
        dl->AddText(center, IM_COL32(255, 255, 255, 255), text);
    }
    bool verifySendUser(std::string sendUser, std::string& msg) {//sendUser, & client.receiveMessage
        int pos1 = msg.find('#');
        if (pos1 == std::string::npos) {
            return false;
        }
        int pos2 = msg.find('#', pos1 + 1);
        if (pos2 == std::string::npos) {
            return false;
        }
        if (msg.substr(pos1 + 1, pos2 - pos1 - 1) == username) {
            msg = msg.substr(pos2 + 1, msg.size() - 1);
            return true;
        }
        return false;

    }
    bool isAllMsg(std::string& msg) {
        int pos1 = msg.find('#');
        if (pos1 == std::string::npos) {
            return false;
        }
        int pos2 = msg.find('#', pos1 + 1);
        if (pos2 == std::string::npos) {
            return false;
        }
        if (msg.substr(pos1 + 1, pos2 - pos1 - 1) == "all") {
            msg = msg.substr(pos2 + 1, msg.size() - 1);
            return true;
        }
        return false;
    }
};
class Sound {
public:
    static Sound& getInstance() {
        static Sound instance; // Lazy initialisation
        return instance;
    }
    void play() {
        std::cout << "sound play" << std::endl;
        system->playSound(sound, NULL, false, NULL);
    }

    void update() {
        system->update();
    }

private:
    FMOD::System* system = NULL;
    FMOD::Sound* sound = NULL;
    //FMOD::Channel* channel = NULL;
    //Sound() = default; // Private constructor
    Sound() {
        //FMOD::System* system;
        FMOD::System_Create(&system);
        system->init(512, FMOD_INIT_NORMAL, NULL);
        system->createSound("pop.mp3", FMOD_DEFAULT, NULL, &sound);
    }
    ~Sound() {
        system->close();
        system->release();
    }
};
#endif

