#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include "SQLiteCpp/SQLiteCpp.h"
#include <SQLiteCpp/VariadicBind.h>
#include <SoapSeller.h>

#include <tgbot/tgbot.h>

using namespace std;
using namespace TgBot;

int main() {
    string token(getenv("TOKEN"));
    printf("Token: %s\n", token.c_str());

    std::string filename_example_db3("/Users/shankarv/sgn/uthra_01/sgn_uthra_01.db");
    SQLite::Database    db(filename_example_db3); 

    Bot bot(token);
    std::shared_ptr<Bot> pBot = std::make_shared<Bot>(token);

    // Thanks Pietro Falessi for code
    InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
    vector<InlineKeyboardButton::Ptr> row0;
    InlineKeyboardButton::Ptr checkButton(new InlineKeyboardButton);
    checkButton->text = "check";
    checkButton->callbackData = "check";
    row0.push_back(checkButton);
    keyboard->inlineKeyboard.push_back(row0);

    bot.getEvents().onCommand("SGN", [pBot](Message::Ptr message) {
        SoapSeller ss("/Users/shankarv/sgn/uthra_01/sgn_uthra_01.db");
        ss.onViewCommand(pBot, message);
    });

    bot.getEvents().onCommand("start", [&bot, &keyboard](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!", false, 0, keyboard);
    });
    bot.getEvents().onCommand("sqlite", [&bot, &keyboard, &db](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!", false, 0, keyboard);
        SQLite::Statement   query(db, "SELECT * FROM Soap");
        while (query.executeStep()) {
            int id = query.getColumn(0);
            const char *name = query.getColumn(1);
            std::cout << "id " << id << " name " << name << std::endl;
        }
    });
    bot.getEvents().onCommand("check", [&bot, &keyboard](Message::Ptr message) {
        string response = "ok";
        bot.getApi().sendMessage(message->chat->id, response, false, 0, keyboard, "Markdown");
    });
    bot.getEvents().onCallbackQuery([&bot, &keyboard](CallbackQuery::Ptr query) {
        string response = "You have chosen " + query->data;
        bot.getApi().sendMessage(query->message->chat->id, response, false, 0, keyboard, "Markdown");
    });

    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
    });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (exception& e) {
        printf("error: %s\n", e.what());
    }

    return 0;
}
