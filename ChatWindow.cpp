#include "ChatWindow.h"

std::string receiveFrom(std::string& msg) {
    int pos = msg.find(':');
    std::string result = msg.substr(0, pos);
    msg = msg.substr(pos + 1, msg.size() - 1);
    return result;
}


//class MessageBlock---------------------------------
MessageBlock::MessageBlock() {
    num = 0;
    content = "";
    username = "UnknownUser";
    position = ImVec2(0, 0);
	height = 0;
    space = ImVec2(5, 5);
    block_width = 300;
}
void MessageBlock::setBlock(int _num, std::string _content, std::string _username, ImVec2 _position, bool _isUser, const float main_scale, std::string _target) {
	num = _num;
	content = _content;
	username = _username;
	position = _position;
	//height = 50; //default height
	isUser = _isUser;
	name2[0] = username[0]; name2[1] = username[1]; name2[2] = '\0';
    //block_width = 300;
    //calculate content size
    float wrap_w = block_width - space.x * 2.0f;//*2 for left and right
    ImVec2 text_sz = ImGui::CalcTextSize(content.c_str(), nullptr, false, wrap_w);
    height = space.y * 2.0f + text_sz.y;
    //set communicating target
    target = _target;

}
void MessageBlock::drawBlock(const ImGuiWindowFlags& child_flags, bool needIcon) {
    //initialize
    const static ImVec4 msg_bg_user = ImVec4(0.70196f * 0.8, 0.90196f * 0.8, 0.05882f * 0.8, 1.f);//39, 56, 30
    const static ImVec4 msg_bg_other = ImVec4(0.05882f * 0.8, 0.20000f * 0.8, 0.36863f * 0.8, 1.f);//39, 56, 30
	const static ImVec4 msg_text_user = ImVec4(0.05882f, 0.20000f, 0.36863f, 1.f);
	const static ImVec4 msg_text_other = ImVec4(0.70196f, 0.90196f, 0.05882f, 1.f);
    const static ImVec2 icon_pos_offset_user = ImVec2(390, -20);;
    const static ImVec2 icon_pos_offset_other = ImVec2(-40, -20);;
    const static ImVec2 content_pos_offset_user = ImVec2(85, 0);;
    const static ImVec2 content_pos_offset_other = ImVec2(-5, 0);

    //if needIcon == false, it is a private message!
    const static ImVec4 msg_dm_bg_user = ImVec4(0.820f * 0.8, 0.286f * 0.8, 0.824f * 0.8, 1.0f);
    const static ImVec4 msg_dm_bg_other = ImVec4(0.431f * 0.8, 0.129f * 0.8, 1.000f * 0.8, 1.0f);
    const static ImVec4 msg_dm_text_user = ImVec4(0.356f, 0.976f, 0.804f, 1.0f);
    const static ImVec4 msg_dm_text_other = ImVec4(0.356f, 0.976f, 0.804f, 1.0f);
    const static ImVec2 content_dm_pos_offset_user = ImVec2(85, 0);;
    const static ImVec2 content_dm_pos_offset_other = ImVec2(-30, 0);
    //set properties
    ImVec4 msg_bg;
    ImVec4 msg_text;
    ImVec2 icon_pos;
    ImVec2 content_pos;
    if (isUser) {
        if (needIcon) {
            msg_bg = msg_bg_user;
            msg_text = msg_text_user;
            icon_pos = ImVec2(position.x + icon_pos_offset_user.x, position.y + icon_pos_offset_user.y);
            content_pos = ImVec2(position.x + content_pos_offset_user.x, position.y + content_pos_offset_user.y);
        }
        else {
            msg_bg = msg_dm_bg_user;
            msg_text = msg_dm_text_user;
            icon_pos = ImVec2(position.x + icon_pos_offset_user.x, position.y + icon_pos_offset_user.y);
            content_pos = ImVec2(position.x + content_dm_pos_offset_user.x, position.y + content_dm_pos_offset_user.y);
        }
    }
    else {
        if (needIcon) {
            msg_bg = msg_bg_other;
            msg_text = msg_text_other;
            icon_pos = ImVec2(position.x + icon_pos_offset_other.x, position.y + icon_pos_offset_other.y);
            content_pos = ImVec2(position.x + content_pos_offset_other.x, position.y + content_pos_offset_other.y);
        }
        else {
            msg_bg = msg_dm_bg_other;
            msg_text = msg_dm_text_other;
            icon_pos = ImVec2(position.x + icon_pos_offset_other.x, position.y + icon_pos_offset_other.y);
            content_pos = ImVec2(position.x + content_dm_pos_offset_other.x, position.y + content_dm_pos_offset_other.y);
        }
    }
    ImGui::PushStyleColor(ImGuiCol_ChildBg, msg_bg);
    ImGui::SetCursorPos(content_pos);
    ImGuiID id = ImGui::GetID(("MessageBlock_" + std::to_string(num)).c_str());

    if (ImGui::BeginChild(id, ImVec2(block_width, height), false, child_flags)) {//I set main_scale = 1, may cause some problem if user change the resolution
        ImGui::PushStyleColor(ImGuiCol_Text, msg_text);
        ImGui::SetCursorPos(space);

        float wrap_w = block_width - space.x * 2.0f;//*2 for left and right
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + wrap_w);
        ImGui::TextUnformatted(content.c_str());
        ImGui::PopTextWrapPos();
        ImGui::PopStyleColor(1);
        
    }
    ImGui::EndChild();
    ImGui::PopStyleColor(1);
    if (needIcon) {
        ImGui::SetCursorPos(ImVec2(icon_pos.x, icon_pos.y + height));
        drawIcon();
    }
    
}
void MessageBlock::drawIcon() {
    static ImU32 userColor = IM_COL32(19, 51, 94, 255);
    static ImU32 otherColor = IM_COL32(74 * 0.8, 224 * 0.8, 255 * 0.8, 255);
    static ImU32 hoveredColor = IM_COL32(168, 226, 237, 255);
    static ImU32 borderColor = IM_COL32(255, 255, 255, 255);

    const static ImVec4 iconText = ImVec4(0.05882f * 0.6, 0.20000f * 0.6, 0.36863f * 0.6, 1.f);//39, 56, 30
    ImVec2 size(30, 30);
    bool clicked = false;
    std::string id = username + "##" + std::to_string(num);//"##" can make a unique id
    if (ImGui::InvisibleButton(id.c_str(), size)) {
        clicked = !clicked;
    }
    bool hovered = ImGui::IsItemHovered();

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();

    ImU32 fill;
    if (!hovered) {
        fill = isUser ? IM_COL32(19, 51, 94, 255) : IM_COL32(74 * 0.8, 224 * 0.8, 255 * 0.8, 255);
    }
    else {
        fill = hoveredColor;
    }

    if (clicked) {
        fill = IM_COL32(255, 255, 255, 255);
        std::cout << "Clicked on user: " << username << std::endl;
        //we can do DM function here, for example, open a new chat window with this user
    }

    dl->AddRectFilled(p0, p1, fill, 4.0f);
    dl->AddRect(p0, p1, hoveredColor, 4.0f, 0, 1.0f);

    ImGui::SetItemTooltip(username.c_str());

    //add name
    const char* text = name2;
    ImVec2 text_size = ImGui::CalcTextSize(text);
    ImVec2 center = ImVec2(
        p0.x + (size.x - text_size.x) * 0.5f,
        p0.y + (size.y - text_size.y) * 0.5f
    );
    dl->AddText(center, IM_COL32(255, 255, 255, 255), text);
}



//class ChatWindow---------------------------------
std::string getNewUserId(std::string msg) {
    std::string newUserName;
    newUserName = msg.substr(8, msg.size() - 1);
    return newUserName;
}
//void getUserList(OnlineUsers& users, std::string msg) {
//
//    std::stringstream ss(msg);
//    std::string item;
//    while (std::getline(ss, item, '#')) { 
//        if (!item.empty()) {
//            users.names.push_back(item);
//            std::cout << item << std::endl;
//        }
//    }
//}


ChatWindow::ChatWindow() {
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;

    child_flags |= ImGuiWindowFlags_NoTitleBar;
    child_flags |= ImGuiWindowFlags_NoMove;
    child_flags |= ImGuiWindowFlags_NoResize;
    child_flags |= ImGuiWindowFlags_NoScrollbar;
}
void ChatWindow::addMessage(const std::string& content, const std::string& username, bool isUser, const float main_scale, std::string _target) {
    MessageBlock newBlock;
    static float space = 2;//8
	//set block content

    int prevBlockHeightPos = 0;
    for (int i = messageBlocks.size() - 1; i >= 0; i--) {
        if (messageBlocks[i].getTarget() == _target) {
            prevBlockHeightPos = messageBlocks[i].getBlockHeightPos() ;
            int prevBlockHeight = messageBlocks[i].getHeight();
            newBlock.setBlockHeightPos(prevBlockHeight + prevBlockHeightPos + space);
            break;
        }
    }
    newBlock.setBlock((int)messageBlocks.size(), content, username, ImVec2(50, newBlock.getBlockHeightPos() + 10), isUser, main_scale, _target);

    messageBlocks.push_back(newBlock);
}
void ChatWindow::render(Client& client, bool& hasName) {
    //size and position
    ImVec2 pos = { 0, 0 };
    ImVec2 nSize = { 560, 800 };
    ImGui::SetWindowPos("ChatRoomWindow", pos);
    ImGui::SetWindowSize("ChatRoomWindow", nSize);
    //color settings
    const static ImVec4 chat_bg = ImVec4(0.1f, 0.14f, 0.1f, 1.00f);
    const static ImVec4 msg_bg = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
    const static ImVec4 input_bg = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
    const static ImVec4 login_bg = ImVec4(0.29020f * 0.8, 0.87843f * 0.8, 1.00000f * 0.8, 1.f);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, chat_bg);
    if (ImGui::Begin("ChatRoomWindow", 0, window_flags)) {

        if (!hasName){//if this is the first time launch this appication
            ImGui::PushStyleColor(ImGuiCol_ChildBg, login_bg);
            ImGui::SetCursorPos(ImVec2(0, 0));
            static bool needConnect = true;
            static bool connectSuccess = false;
            if (needConnect) {
                connectSuccess = client.tryConnect();
                needConnect = false;
            }

            if (ImGui::BeginChild("##Login", ImVec2(500, 780), false, 0)) {
                if (connectSuccess) {
                    needConnect = false;
                    float avail_w = ImGui::GetContentRegionAvail().x;

                    auto CenterText = [&](const char* txt) {//make text center
                        ImVec2 ts = ImGui::CalcTextSize(txt);
                        float x = (avail_w - ts.x) * 0.5f;
                        if (x > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x);
                        ImGui::TextUnformatted(txt);
                        };
                    //We have at least 2 ways to set postion: Dummy() and SetCursorPos()
                    ImGui::Dummy(ImVec2(0.0f, 200.0f));

                    ImGui::SetWindowFontScale(2.0f);
                    CenterText("WELCOME!!");

                    ImGui::SetWindowFontScale(1.5f);
                    CenterText("Enter Your Name Here:");
                    ImGui::SetWindowFontScale(1.0f);

                    ImGui::Dummy(ImVec2(0.0f, 12.0f));
                    static char buf[256] = "";
                    float input_w = avail_w * 0.60f;
                    float start_x = (avail_w - input_w) * 0.5f;
                    if (start_x > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + start_x);
                    ImGui::SetWindowFontScale(1.35f);
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));
                    ImGui::SetNextItemWidth(input_w);
                    ImGui::PopStyleVar();
                    ImGui::SetWindowFontScale(1.0f);
                    static std::string nameValid = "";
                    static bool launchedThread = false;
                    if (!launchedThread) {
                        launchedThread = true;
                        client.startSend();
                        client.startReceive();
                    }
                    if (ImGui::InputText("##name", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_EnterReturnsTrue)) {
                        if (buf[0] != '\0') {
                            client.sendName(buf);
                        }
                    }
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05882f * 0.8, 0.20000f * 0.8, 0.36863f * 0.8, 1.f));
                    if (ImGui::Button("Login", ImVec2(50, 25))) {//button same function
                        if (buf[0] != '\0') {
                            client.sendName(buf);
                        }
                    }
                    if (client.sameName) {
                        nameValid = "There is a same name in this chat!!";
                    }
                    if (client.login) {
                        hasName = true;
                        setUsername(client.name);
                    }
                    ImGui::PopStyleColor(1);
                    ImGui::SetCursorPosX(100);
                    ImGui::Text(nameValid.c_str());

                }
                else {
                    needConnect = false;
                    float avail_w = ImGui::GetContentRegionAvail().x;

                    auto CenterText = [&](const char* txt) {
                        ImVec2 ts = ImGui::CalcTextSize(txt);
                        float x = (avail_w - ts.x) * 0.5f;
                        if (x > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x);
                        ImGui::TextUnformatted(txt);
                        };

                    ImGui::Dummy(ImVec2(0.0f, 200.0f));

                    ImGui::SetWindowFontScale(2.0f);
                    CenterText("CONNECT FAILED");


                    ImGui::SetCursorPosX(200);
                    if (ImGui::Button("Retry")) {
                        needConnect = true;
                    }
                    ImGui::SetWindowFontScale(1.0f);
                }
            }

            //----------------copy from ImGui_demo.cpp
            struct Funcs
            {
                static float Sin(float i) { return sinf(i); }
                static float Const(float i) { return 0; }
            };
            float (*func)(float) = connectSuccess ? Funcs::Sin : Funcs::Const;
            //decoration
            static float values[90] = {};
            static int values_offset = 0;
            static double refresh_time = 0.0;
            if (refresh_time == 0.0)
                refresh_time = ImGui::GetTime();
            while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
            {
                static float phase = 0.0f;
                values[values_offset] = func(phase);
                values_offset = (values_offset + 1) % IM_COUNTOF(values);
                phase += 0.10f;
                refresh_time += 1.0f / 60.0f;
            }

            ImGui::SetCursorPos(ImVec2(20,470));
            ImGui::SetWindowFontScale(1.2f);
            ImGui::Text("Status:");
            ImGui::SameLine();
            if (connectSuccess) ImGui::Text("Online");
            else ImGui::Text("Offline");
            ImGui::SetWindowFontScale(1.f);

            ImGui::BeginDisabled(true);
            ImGui::SetCursorPosY(490);
            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1, 1, 1, 1));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            ImGui::PlotLines("##Dec", values, IM_COUNTOF(values), values_offset, nullptr, -1.0f, 1.0f, ImVec2(600, 200.0f));
            ImGui::PopStyleColor(2);
            ImGui::EndDisabled();
            //-----------------

            ImGui::EndChild();
            ImGui::PopStyleColor(1);
            
        }
        else {
            static OnlineUsers users(username);
            static bool needScroll = false;
            ImGui::PushStyleColor(ImGuiCol_ChildBg, msg_bg);
            ImGui::SetCursorPos(ImVec2(0, 0));
            if (ImGui::BeginChild("Messages", ImVec2(nSize.x - 50, nSize.y - 200), false, window_flags)) {//size of message area
                //draw all message blocks
                for (size_t i = 0; i < messageBlocks.size(); i++) {
                    if(messageBlocks[i].getTarget() == "all")
                        messageBlocks[i].drawBlock(child_flags);
                }
                //scroll to the latest input
                ImGui::Dummy(ImVec2(0.0f, 1.0f));
                if (needScroll) {
                    ImGui::SetScrollHereY(1.0f);
                    needScroll = false;
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleColor(1);

            ImGui::PushStyleColor(ImGuiCol_ChildBg, input_bg);
            ImGui::SetCursorPos(ImVec2(0, ImGui::GetCursorPosY() + 2));

            if (ImGui::BeginChild("Input", ImVec2(nSize.x - 50, nSize.y - ImGui::GetCursorPosY() - 10), false, child_flags)) {
                static char buf[256] = "";
                static bool focus_next = true;

                if (focus_next) {
                    ImGui::SetKeyboardFocusHere();
                    focus_next = false;
                }

                ImGui::SetNextItemWidth(400);
                ImGui::SetCursorPos(ImVec2(20, ImGui::GetCursorPosY() + 20));
                if (ImGui::InputText("##chat_input", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    if (buf[0] != '\0') {
                        addMessage(buf, username, true);//
                        client.inputNew = true;
                        client.sendMessage = buf;
                        client.sendMessage = "#all#" + client.sendMessage;
                        needScroll = true;
                        buf[0] = '\0';
                    }
                    focus_next = true;
                }
                ImGui::SameLine();
                //add a button
                if (ImGui::Button("Send", ImVec2(50, 25))) {//button
                    if (buf[0] != '\0') {
                        if (buf[0] != '\0') {
                            addMessage(buf, username, true);//
                            client.inputNew = true;
                            client.sendMessage = buf;
                            client.sendMessage = "#all#" + client.sendMessage;
                            needScroll = true;
                            buf[0] = '\0';
                        }
                        focus_next = true;
                    }
                }
            }

            ImGui::EndChild();
            ImGui::PopStyleColor(1);

            //Online User
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.3f,0.3f,0.3f,1.f));
            ImGui::SetCursorPos(ImVec2(500, 0));
            if (ImGui::BeginChild("OnlineUsers"), ImVec2(90, 500), false, child_flags) {
                ImGui::Text("users:");
                
                for (int i = 0; i < users.users.size(); i++) {
                    ImGui::SetCursorPosX(7);
                    drawIcon(users.users[i]);
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleColor(1);

            //open a new window if "hasChildWindow" == true
            //if (true) {//for now
            if (hasChildWindow) {
                auto it = std::find_if(users.users.begin(), users.users.end(), [&](const auto& p) { return p.first == dmTarget; });
                if (it != users.users.end()) {
                    it->second = false;
                }
                static ImVec2 childPos = { 570, 50 };
                static ImVec2 childSize = { 450, 600 };
                static bool childNeedScroll = false;
                std::string windowName = "DM-" + dmTarget;
                ImGui::SetWindowPos(windowName.c_str(), childPos);
                ImGui::SetWindowSize(windowName.c_str(), childSize);
                if (ImGui::Begin(windowName.c_str(), 0, window_flags)) {
                    
                    //draw message area
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, msg_bg);
                    ImGui::SetCursorPos(ImVec2(0, 0));
                    ImGui::Text(windowName.c_str());
                    ImGui::SetCursorPos(ImVec2(0, 20));
                    if (ImGui::BeginChild("PrivateMessages", ImVec2(childSize.x, childSize.y - 100), false, child_flags)) {//size of message area
                        //draw all message blocks
                        for (size_t i = 0; i < messageBlocks.size(); i++) {
                            if (messageBlocks[i].getTarget() == dmTarget)
                                messageBlocks[i].drawBlock(child_flags, false);
                        }
                        //scroll to the latest input
                        ImGui::Dummy(ImVec2(0.0f, 1.0f));
                        if (childNeedScroll) {
                            ImGui::SetScrollHereY(1.0f);
                            childNeedScroll = false;
                        }
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleColor(1);

                    ImGui::PushStyleColor(ImGuiCol_ChildBg, input_bg);
                    ImGui::SetCursorPos(ImVec2(0, ImGui::GetCursorPosY() + 2));

                    if (ImGui::BeginChild("Input", ImVec2(childSize.x, childSize.y - ImGui::GetCursorPosY() - 10), false, child_flags)) {
                        static char buf[256] = "";
                        static bool focus_next = true;

                        if (focus_next) {
                            ImGui::SetKeyboardFocusHere();
                            focus_next = false;
                        }

                        ImGui::SetNextItemWidth(350);
                        ImGui::SetCursorPos(ImVec2(20, ImGui::GetCursorPosY() + 20));
                        if (ImGui::InputText("##chat_input", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_EnterReturnsTrue)) {
                            if (buf[0] != '\0') {
                                //this is a private msg
                                addMessage(buf, username, true, 1, dmTarget);
                                std::cout << "this is a private msg, send to " << dmTarget << std::endl;
                                client.inputNew = true;
                                client.sendMessage = buf;
                                client.sendMessage = "#" + dmTarget + "#" + client.sendMessage;
                                needScroll = true;
                                buf[0] = '\0';
                            }
                            focus_next = true;
                        }
                        ImGui::SameLine();
                        //add a button
                        if (ImGui::Button("Send", ImVec2(50, 25))) {//button
                            if (buf[0] != '\0') {
                                if (buf[0] != '\0') {
                                    //this is a private msg
                                    addMessage(buf, username, true, 1, dmTarget);
                                    std::cout << "this is a private msg, send to" << dmTarget << std::endl;
                                    client.inputNew = true;
                                    client.sendMessage = buf;
                                    client.sendMessage = "#" + dmTarget + "#" + client.sendMessage;
                                    needScroll = true;
                                    buf[0] = '\0';
                                }
                                focus_next = true;
                            }
                        }
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleColor(1);

                }
                ImGui::End();
            }


            //check new message receive
            if (client.receiveNew) {
                client.receiveNew = false;
                //message prepare----client.receiveMessage-> #system:Welcome-jamie  ;  jamie:hello everone    ;    jamie:#jamie#hello mike
                std::string sendUser = receiveFrom(client.receiveMessage);//Welcome-jamie ; hello everone ; #jamie#hello mike
                if (sendUser == "#system") {//if it is a system msg, do with it
                    std::cout << "(system)" << client.receiveMessage << std::endl;
                    if (client.receiveMessage.compare(0, 8, "Welcome-") == 0) {
                        users.addUser(getNewUserId(client.receiveMessage));
                    }
                    if (client.receiveMessage.compare(0, 6, "Users-") == 0) {
                        users.getUserList(client.receiveMessage.substr(6, client.receiveMessage.size() - 1));
                    }
                    if (client.receiveMessage.compare(0, 11, "Disconnect-") == 0) {
                        users.eraseUser(client.receiveMessage.substr(11, client.receiveMessage.size() - 1));
                    }
                }
                else {
                    if (sendUser != username) {//sendUser = jamie ;  hello everone  ;  #jamie#hello mike
                        if (isAllMsg(client.receiveMessage)) {
                            addMessage(client.receiveMessage, sendUser, false);
                            Sound::getInstance().play();
                        }
                        else {
                            //check whether it is a private msg----  sendUser = jamie ; client.receiveMessage = #jamie#hello
                            if (verifySendUser(sendUser, client.receiveMessage)) {
                                addMessage(client.receiveMessage, sendUser, false, 1, sendUser);
                                
                                for (auto& p : users.users) {
                                    if (p.first == sendUser) {
                                        p.second = true;
                                        break;
                                    }
                                }


                                Sound::getInstance().play();
                            }
                        }                      
                    }
                }

            }

        }

        
    }
    ImGui::End();
    ImGui::PopStyleColor(1);
    return;
}
