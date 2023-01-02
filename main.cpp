#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <openssl/evp.h>
#include <openssl/sha.h>

#include <dpp/dpp.h>

#include "json.hpp"

using json = nlohmann::json;

namespace fs = std::filesystem;

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

std::string GetHexRepresentation(const unsigned char *Bytes, size_t Length)
{
    std::string ret;
    ret.reserve(Length * 2);
    for(const unsigned char *ptr = Bytes; ptr < Bytes+Length; ++ptr) {
        char buf[3];
        sprintf(buf, "%02x", (*ptr)&0xff);
        ret += buf;
    }
    return ret;
}

string to_sha1(string org)
{
    char data[org.size()+1];
    strcpy(data, org.c_str());
    size_t length = strlen(data);
    unsigned char hash[20];

    SHA1((unsigned char *)data, length, hash);
    return GetHexRepresentation(hash, 20);
}

std::vector<MortalQuest> getMortalQuests(const std::string& filePath)
{
    std::vector<MortalQuest> quests;
    // Vérifie que le dossier existe
    if (!fs::exists(filePath))
    {
        std::cerr << "Le dossier " << filePath << " n'existe pas." << std::endl;
        return quests;
    }

    // Parcourt tous les fichiers dans le dossier
    for (const auto& entry : fs::directory_iterator(filePath))
    {
        cout << entry.path().stem().string();

        // Ignore les entrées qui ne sont pas des fichiers ou qui ne sont pas des fichiers JSON
        if (!entry.is_regular_file() || entry.path().extension() != ".json")
            continue;

        // Vérifie que le nom du fichier commence par "moq_"
        if (entry.path().stem().string().substr(0, 4) != "moq_")
            continue;

        // Ouvre le fichier en lecture
        std::ifstream file(entry.path());
        if (!file.is_open())
        {
            std::cerr << "Erreur lors de l'ouverture du fichier " << entry.path() << std::endl;
            continue;
        }

        // Parse le contenu du fichier en JSON
        json data;
        file >> data;

        // Pour chaque entrée dans le tableau "quests", ajoute une MortalQuest au vecteur
        for (const auto& quest : data["quests"])
        {
            MortalQuest mortalQuest;
            mortalQuest.name = quest["name"];
            mortalQuest.description = quest["description"];
            mortalQuest.quantity = quest["quantity"];
            mortalQuest.puntos = quest["puntos"];
            quests.push_back(mortalQuest);
        }
    }

    return quests;
}

int main(int argc, char * argv[])
{
    std::string token = argv[1];
    dpp::cluster bot(token,  dpp::i_all_intents );
 
    bot.on_log(dpp::utility::cout_logger());
 
    bot.on_slashcommand([&bot](const dpp::slashcommand_t & event) {
        if (event.command.get_command_name() == "quest") {

            std::string name = std::get<std::string>(event.get_parameter("name"));
            std::string description = std::get<std::string>(event.get_parameter("description"));
            int quantity = std::get<std::int64_t>(event.get_parameter("quantity"));
            int puntos = std::get<std::int64_t>(event.get_parameter("puntos"));

            MortalQuest moquest = create_moquest(name, description, quantity, puntos);

            json j { moquest };
            
            string sha1 = to_sha1(moquest.name);
            cout << "SHA1 : " + moquest.name + " => " +  sha1 << endl;
            string fileH = filePath + "quests/moq_" + sha1 + ".json";
            std::ofstream outfile(fileH);
            outfile << j;
            outfile.close();
            cout << fileH + " created." << endl;

            event.reply(std::string("Quest Added - "
            + std::string("n: ") + name
            + std::string(" d:") + description
            + std::string(" f:") + fileH
            ));
        }
        else if(event.command.get_command_name() == "-quest") {
            std::string name = std::get<std::string>(event.get_parameter("name"));
            string sha1 = to_sha1(name);
            cout << "SHA1 : " + name + " => " +  sha1 << endl;
            string fileH = filePath + "quests/moq_" + sha1 + ".json";
            if( remove(fileH.c_str()) != 0 ) {
                perror( "Error deleting quest" );
                cout << "Can't delete " + fileH << endl;
            }
            else
                puts( "Quest successfully deleted");
                event.reply(std::string("Quest Deleted - "
                    + std::string("n: ") + name
                ));
        }
        else if(event.command.get_command_name() == "listquest") {
            cout << "WTF?" << endl;

            std::string fileH = filePath + "quests/";

            vector<MortalQuest> quests = getMortalQuests("/home/stupidcoyote/Desktop/C++/MortalBot/build/quests/");

            std::string questNames = "";
            for (const auto& quest : quests)
                questNames += quest.name + " ";

            cout << "names:" << questNames << endl;
            event.reply(std::string("Quest List - "
            + questNames
            ));
        }
                
    });
 
    bot.on_ready([&bot](const dpp::ready_t & event) {
        if (dpp::run_once<struct register_bot_commands>()) {

            // /quest 
            dpp::slashcommand addQuestCommand("quest", "Nouvelle quete cree.", bot.me.id);
            addQuestCommand.add_option(
                dpp::command_option(dpp::co_string, "name", "Nom de la quete.", true).
                set_min_length(4).set_max_length(20)
            );
            addQuestCommand.add_option(
                dpp::command_option(dpp::co_string, "description", "Description de la quete.", true).
                set_min_length(4).set_max_length(250)
            );
            addQuestCommand.add_option(
                dpp::command_option(dpp::co_integer, "quantity", "Nombre de completion maximum.", true).
                set_min_value(0).set_max_value(INT_MAX)
            );
            addQuestCommand.add_option(
                dpp::command_option(dpp::co_integer, "puntos", "Nombre de puntos comme recompense.", true).
                set_min_length(0).set_max_length(INT_MAX)
            );

            // -quest
            dpp::slashcommand deleteQuestCommand("-quest", "Quete suprimee.", bot.me.id);
            deleteQuestCommand.add_option(
                dpp::command_option(dpp::co_string, "name", "Nom de la quete a supprimer.", true).
                set_min_length(4).set_max_length(20)
            );

            // listquest
            dpp::slashcommand listQuestsCommand("listquest", "La liste des quetes disponibles.", bot.me.id);

            bot.global_command_create(addQuestCommand);
            bot.global_command_create(deleteQuestCommand);
            bot.global_command_create(listQuestsCommand);

        }
    });

    cout << "Bot starting." << endl;
 
    bot.start(dpp::st_wait);
 
    return 0;
}