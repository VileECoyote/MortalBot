#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <openssl/evp.h>

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

string to_sha1(string org)
{
    std::stringstream result;
    EVP_MD_CTX *mdctx;  
    const EVP_MD *md;  
    unsigned char md_value[EVP_MAX_MD_SIZE];  
    unsigned int md_len;  
  
    md = EVP_sha1();  
    mdctx = EVP_MD_CTX_create();  
    EVP_DigestInit_ex(mdctx, md, NULL);  
    EVP_DigestUpdate(mdctx, org.c_str(), sizeof(org)-1);  
  
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);  
    EVP_MD_CTX_destroy(mdctx);

    for (uint i = 0; i < md_len; i++){
        result << hex << (int)md_value[i];
    }

    EVP_cleanup();  
    return result.str();
}

int main(int argc, char * argv[])
{
    std::string token = argv[1];
    dpp::cluster bot(token,  dpp::i_all_intents );
 
    bot.on_log(dpp::utility::cout_logger());
 
    /* The event is fired when someone issues your commands */
    bot.on_slashcommand([&bot](const dpp::slashcommand_t & event) {
        /* Check which command they ran */
        if (event.command.get_command_name() == "quest") {
            /* Fetch a parameter value from the command parameters */
            std::string name = std::get<std::string>(event.get_parameter("name"));
            std::string description = std::get<std::string>(event.get_parameter("description"));
            std::int quantity = std::get<std::int64_t>(event.get_parameter("quantity"));
            std::int puntos = std::get<std::int64_t>(event.get_parameter("puntos"));

            MortalQuest moquest = create_moquest(name, description, quantity, puntos);

            json j { moquest };
            string sha1 = to_sha1(moquest.name);
            string fileH = filePath + "quests/moq_" + sha1 + ".json";
            std::ofstream outfile(fileH);
            outfile << j;
            outfile.close();
            /* Reply to the command. There is an overloaded version of this
            * call that accepts a dpp::message so you can send embeds.
            */
            event.reply(std::string("n:") + name
            + std::string(" d:") + description
            + std::string(" f:") + fileH
            );
        }
    });
 
    bot.on_ready([&bot](const dpp::ready_t & event) {
        if (dpp::run_once<struct register_bot_commands>()) {
 
            /* Create a new global command on ready event */
            dpp::slashcommand newcommand("quest", "Nouvelle quete cree.", bot.me.id);
            newcommand.add_option(
                dpp::command_option(dpp::co_string, "name", "Nom de la quete.", true).
                set_min_length(4).set_max_length(20)
            );
            newcommand.add_option(
                dpp::command_option(dpp::co_string, "description", "Description de la quete.", true).
                set_min_length(4).set_max_length(250)
            );
            newcommand.add_option(
                dpp::command_option(dpp::co_integer, "quantity", "Nombre de completion maximum.", true).
                set_min_value(0).set_max_value(INT_MAX)
            );
            newcommand.add_option(
                dpp::command_option(dpp::co_integer, "puntos", "Nombre de puntos comme recompense.", true).
                set_min_length(0).set_max_length(INT_MAX)
            );
            /* Register the command */
            bot.global_command_create(newcommand);
        }
    });

    cout << "Bot starting." << endl;
 
    bot.start(dpp::st_wait);
 
    return 0;
}