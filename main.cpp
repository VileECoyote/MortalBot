#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

#include <dpp/dpp.h>

#include "json.hpp"

using json = nlohmann::json;

using namespace std;

string filePath = "";

struct MortalQuest{
    string name;
    string description;
    int quantity;
    int puntos;
};

struct MortalUser{
    dpp::snowflake discordId;
    string username;
    int puntos;
};

void to_json(json& j, const MortalQuest & q)
{
    j = json { "MortalQuest", {
        { "name", q.name },
        { "description", q.description },
        { "quantity", q.quantity },
        { "puntos", q.puntos },
    } };
}

void questfile_default()
{
    MortalQuest defaultQuest;
    defaultQuest.name = "default";
    defaultQuest.description = "Kill Zoups in the most unexpected way.";
    defaultQuest.quantity = 1;
    defaultQuest.puntos = 42;

    json j { defaultQuest };

    std::ofstream outfile(filePath + "/mortalquests.json");

    outfile << to_string(j);

    outfile.close();

    cout << "Default quest file created." << endl;
}

void questfile_loading()
{
    cout << "Quest file found." << endl;
}

void quest_creator()
{
    bool fileExists = filesystem::exists(filePath + "/mortalquests.json");

    if (!fileExists)
        questfile_default();
    else
        questfile_loading();
}

void start_bot(dpp::cluster *bot) {
	cout << "Bot starting." << endl;

	bot->on_message_create([bot](const dpp::message_create_t &event) {
		if (event.msg.content == "papers") {
			cout << event.msg.author.username + " requested papers" << endl;
			bot->message_create(dpp::message(
				event.msg.channel_id,
				"username:" + event.msg.author.username +
					" snowflake:" + to_string(event.msg.author.id)));
		} else if (event.msg.content.rfind("addquest", 0) == 0) {
            cout << event.msg.author.username + " requested addquest" << endl;
            char * cstr = new char [event.msg.content.length()+1];
            std::strcpy (cstr, event.msg.content.c_str());
            char * p = std::strtok (cstr," ");
            p = std::strtok(NULL," ");
            cout << "name " << p << endl;
            p = std::strtok(NULL," ");
            cout << "description " << p << endl;
            p = std::strtok(NULL," ");
            cout << "quantity " << p << endl;
            p = std::strtok(NULL," ");
            cout << "puntos " << p << endl;
        }
	});

    dpp::commandhandler command_handler(bot);
    command_handler.add_prefix(".").add_prefix("/");

    bot->on_ready([&command_handler](const dpp::ready_t &event) {
 
        command_handler.add_command(
            /* Command name */
            "quests",
 
            /* Parameters */
            {
                {"Name", dpp::param_info(dpp::pt_string, false, "Nom de la quete.") }
                ,{"Description", dpp::param_info(dpp::pt_string, false, "Description de la quete.") }
                ,{"Quantity", dpp::param_info(dpp::pt_integer, false, "Nombre de completion maximum.") }
                ,{"Puntos", dpp::param_info(dpp::pt_integer, false, "Nombre de puntos comme recompense.") }
            },
 
            /* Command handler */
            [&command_handler](const std::string& command, const dpp::parameter_list_t& parameters, dpp::command_source src) {
                std::string name;
                std::string description;
                std::int64_t quantity;
                std::int64_t puntos;
                if (!parameters.empty()) {
                    name = std::get<std::string>(parameters[0].second);
                    description = std::get<std::string>(parameters[1].second);
                    quantity = std::get<std::int64_t>(parameters[2].second);
                    puntos = std::get<std::int64_t>(parameters[3].second);
                }
                command_handler.reply(dpp::message("Quest Name:\"" + name 
                + "\" Description:\"" + description
                + "\" Quantity:" + to_string(quantity)
                + " Puntos" + to_string(puntos)
                ), src);
            },
 
            /* Command description */
            "A quest command"
        );
 
        /* NOTE: We must call this to ensure slash commands are registered.
         * This does a bulk register, which will replace other commands
         * that are registered already!
         */
        command_handler.register_commands();
    });

	//quest_creator();
    
    bot->start();
	cout << "Bot started." << endl;
}

string test_json(string n, string d, int q, int p)
{
    MortalQuest defaultQuest;
    defaultQuest.name = n;
    defaultQuest.description = d;
    defaultQuest.quantity = q;
    defaultQuest.puntos = p;

    json j { defaultQuest };

    return to_string(j);
}

int main(int argc, char * argv[])
{
    std::string token = argv[1];
    dpp::cluster bot(token,  dpp::i_all_intents );
 
    bot.on_log(dpp::utility::cout_logger());
 
    dpp::commandhandler command_handler(&bot);
    command_handler.add_prefix(".").add_prefix("/");
    bot.on_ready([&command_handler](const dpp::ready_t &event) {

        vector<std::pair<string, dpp::param_info>> params;

        params.push_back({"name", dpp::param_info(dpp::pt_string, false, "Nom de la quete.")});
        params.push_back({"description", dpp::param_info(dpp::pt_string, false, "Description de la quete.")});
        params.push_back({"quantity", dpp::param_info(dpp::pt_integer, false, "Nombre de completion maximum.")});
        params.push_back({"puntos", dpp::param_info(dpp::pt_integer, false, "Nombre de puntos comme recompense.")});

        command_handler.add_command(
            /* Command name */
            "quest",
            params,
            /* Command handler */
            [&command_handler](const std::string& command, const dpp::parameter_list_t& parameters, dpp::command_source src) {
                if (parameters.size() == 4) {
                    //stringstream reply;
                    //reply << "name: " << std::get<std::string>(parameters[0].second)
                    //<< endl << "description: " << std::get<std::string>(parameters[1].second)
                    //<< endl << "quantity: " << std::to_string(std::get<std::int64_t>(parameters[2].second))
                    //<< endl << "puntos: " << std::to_string(std::get<std::int64_t>(parameters[3].second));

                    //command_handler.reply(dpp::message(reply.str()), src);

                    string reply = test_json(std::get<std::string>(parameters[0].second), std::get<std::string>(parameters[1].second)
                    ,std::get<std::int64_t>(parameters[2].second), std::get<std::int64_t>(parameters[3].second));

                    command_handler.reply(dpp::message(reply), src);
                }
                else
                {
                    command_handler.reply(dpp::message("What the fuck?"), src);
                }
            },
 
            /* Command description */
            "Command to add quests."
        );
 
        /* NOTE: We must call this to ensure slash commands are registered.
         * This does a bulk register, which will replace other commands
         * that are registered already!
         */
        command_handler.register_commands();
 
    });
 
    bot.start(dpp::st_wait);
 
    return 0;

    //if (argc != 3)
    //{
    //    std::cout << "Something is missing." << std::endl;
    //    return -1;
    //}
    //
    //std::cout << "Creating Bot and Thread." << std::endl;
    //
    //std::cout << "Filepath is: " << argv[2] << std::endl;
    //filePath = argv[2];
    //
    //std::string cmd;
    //std::string token = argv[1];
    //bool running = true;
    //dpp::cluster bot(token, dpp::i_default_intents | dpp::i_message_content);
    //std::thread bot_life(start_bot, &bot);
    //
    // while (running)
    // {
    //    std::cin >> cmd;
    //
    //    if (cmd == "end")
    //    {
    //        std::cout << "MortalBot shut down requested." << std::endl;
    //        running = false;
    //        bot.shutdown();
    //        bot_life.~thread();
    //        return 0;
    //    }
    // }
}