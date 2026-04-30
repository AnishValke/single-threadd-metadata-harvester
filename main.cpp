#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[]){
    vector<string> args (argv, argv + argc);
    cout << "Blitz-Search CLI Initialized." << "\n";
    cout << "--------------------------------" << "\n";

    if(args.size() < 3){
        cerr << "\033[38;2;255;105;180mError: Missing arguments.\n";
        cerr << "Usage: ./blitz <search_term> <directory_path>\n\033[0m";
        return 1;
    }

    string directoryPath = args[1];
    string targetExtension = args[2];

    if(!targetExtension.empty() && targetExtension[0] != '.'){ //checking for typo
        targetExtension = "." + targetExtension;
    }

    unordered_set<string> validExtensions = {".txt", ".cpp", ".md", ".json", ".h"};

    if (!validExtensions.count(targetExtension)) { //cerr for invalid extensions
        cerr << "\033[38;2;255;105;180mFatal Error: Unsupported file extension '" << targetExtension << "'\n";
        cerr << "Supported types: .txt, .cpp, .md, .json, .h\n\033[0m";
        return 1;
    }

    cout << "Initializing Blitz Harvester...\n";
    cout << "Scanning Directory: '" << directoryPath << "' for " << targetExtension << " files.\n";
    cout << "--------------------------------\n";

    unordered_map <string, pair<int, int>> emailStats; //email <to_count, from_count>
    regex toPattern("To:\\s*([a-zA-Z0-9_\\-\\.]+@([a-z]+\\.)+[a-z]{2,3})"); //regex for to
    regex fromPattern("From:\\s*([a-zA-Z0-9_\\-\\.]+@([a-z]+\\.)+[a-z]{2,3})"); //regex for from
    smatch match; //special object for regex strings, stores strings according to () in the regex

    for(const auto& entry : fs::recursive_directory_iterator(directoryPath)){ //recursive iterator DFS in the folder
        if(entry.is_regular_file()){
            if(entry.path().extension().string() == targetExtension){
                ifstream file(entry.path());
                string line;
                while(getline(file, line)){
                    if(regex_search(line, match, toPattern)){
                        string extractedEmail = match[1].str(); //used to convert smatch object to string
                        emailStats[extractedEmail].first++;
                    }
                    else if(regex_search(line, match, fromPattern)){
                        string extractedEmail = match[1].str();
                        emailStats[extractedEmail].second++;
                    }
                }
            }
        }
    }

    cout << "\n\033[36m================ METADATA REPORT ================\033[0m\n";
    for (const auto& entry : emailStats) {
        string email = entry.first;
        int toCount = entry.second.first;
        int fromCount = entry.second.second;

        cout << "\033[33m" << email << "\033[0m\n";
        cout << "\033[32m  -> Received (To): " << toCount << "\033[0m\n";
        cout << "\033[35m  -> Sent (From):   " << fromCount << "\033[0m\n\n";
    }
    
    return 0;
}