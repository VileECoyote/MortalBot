#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <openssl/sha.h>

#include <dpp/dpp.h>

#include "json.hpp"

using json = nlohmann::json;

using namespace std;

string filePath = "./";

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

    std::ofstream outfile(filePath + "quests/mortalquests.json");

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

MortalQuest create_moquest(string n, string d, int q, int p)
{
    MortalQuest quest;
    quest.name = n;
    quest.description = d;
    quest.quantity = q;
    quest.puntos = p;

    return quest;
}

void to_sha1(string org)
{
    unsigned char hash[20];

    SHA1((unsigned char*)org.c_str(), org.size(), hash);

    std::stringstream shastr;
    for (const auto &byte: hash)
    {
        shastr << std::setw(2) << (char)byte;
    }
    cout << shastr.str() << endl;
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
        "quest",
        params,
        [&command_handler](const std::string& command, const dpp::parameter_list_t& parameters, dpp::command_source src) {
            if (parameters.size() == 4) {
                MortalQuest moquest = create_moquest(std::get<std::string>(parameters[0].second), std::get<std::string>(parameters[1].second)
                ,std::get<std::int64_t>(parameters[2].second), std::get<std::int64_t>(parameters[3].second));

                json j { moquest };

                //std::ofstream outfile(filePath + "/quests/" + to_sha1(moquest.name) + ".json");
                //std::ofstream outfile(filePath + "/quests/" + moquest.name + ".json");
                //outfile << j;
                //outfile.close();

                to_sha1(moquest.name);

                command_handler.reply(dpp::message("New quest created : " + moquest.name), src);
            }
        },
        "Command to add quests."
    );
 
        /* NOTE: We must call this to ensure slash commands are registered.
         * This does a bulk register, which will replace other commands
         * that are registered already!
         */
        command_handler.register_commands(); 
    });

    cout << "Bot starting." << endl;
 
    bot.start(dpp::st_wait);
 
    return 0;
}