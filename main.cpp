// main.cpp
#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#else
#include <cstdio> // For popen, pclose
#endif
namespace fs = std::filesystem;

#ifdef _WIN32
bool copyToClipboard(const std::string& text) {
    if (!OpenClipboard(NULL))
        return false;
    EmptyClipboard();
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!hg) {
        CloseClipboard();
        return false;
    }
    memcpy(GlobalLock(hg), text.c_str(), text.size() + 1);
    GlobalUnlock(hg);
    SetClipboardData(CF_TEXT, hg);
    CloseClipboard();
    GlobalFree(hg);
    return true;
}
#else
// Function to copy text to clipboard on Linux using xclip
bool copyToClipboard(const std::string& text) {
    FILE* pipe = popen("xclip -selection clipboard", "w");
    if (!pipe)
        return false;
    fprintf(pipe, "%s", text.c_str());
    pclose(pipe);
    return true;
}
#endif

/**
 * Gets the regular expression object based on the file extension.
 * @param extension The file extension for which the regex needs to be obtained.
 * @return A regex object suitable for identifying functions in the given file type.
 */
std::regex getFunctionRegex(const std::string &extension){
    if (extension == ".py"){
        return std::regex(R"(def\s+[a-zA-Z_]\w*\s*\([^)]*\):)");
    }
    else if (extension == ".js" || extension == ".ts"){
        return std::regex(R"(function|const|let|var\s+[a-zA-Z_]\w*\s*\([^)]*\)(?:\s*=>)?\s*{)");
    }
    else if (extension == ".cpp" || extension == ".c" || extension == ".h"){
        return std::regex(R"([a-zA-Z_]\w*\s+[a-zA-Z_]\w*\s*\([^)]*\)\s*{)");
    }
    // Default regex pattern for other file types
    return std::regex(R"((?:function|def|\=>)?\s*[a-zA-Z_]\w*\s*\([^)]*\)[\s{:]\s*)");
}

/**
 * Determines if a line from a file represents a function definition.
 * @param line The line from the file to be checked.
 * @param extension The extension of the file, used to determine appropriate regex.
 * @return True if the line represents a function definition, false otherwise.
 */
bool isFunctionDefinition(const std::string &line, const std::string &extension){
    std::regex funcPattern = getFunctionRegex(extension);
    return std::regex_search(line, funcPattern);
}

/**
 * Determines if a line from a file represents a return statement.
 * @param line The line from the file to be checked.
 * @return True if the line represents a return statement, false otherwise.
 */
bool isReturnStatement(const std::string &line)
{
    std::regex returnPattern(R"(\s*return\s+.*[;]?)");
    return std::regex_search(line, returnPattern);
}

/**
 * Processes a file to extract and output function names and return statements.
 * @param fileName The name of the file to be processed.
 * @param output_text A reference to a string where the output is accumulated.
 */
void processFile(const std::string &fileName, std::string &output_text){
    #ifdef _WIN32
    std::string dir = fs::current_path().string();
    #else
    std::string dir = fs::current_path();
    #endif

    dir+="/"+fileName;
    std::ifstream file(dir);
    if (!file.is_open()){
        std::cerr << "Failed to open " << fileName << std::endl;
        return;
    }

    bool insideFunction = false;
    output_text += "\nFunction Names from File: " + fileName + ":\n";
    std::string line;

    while (std::getline(file, line)){
        if (isFunctionDefinition(line, fileName.substr(fileName.find_last_of(".") + 1))){
            insideFunction = true;
            output_text += line + "\n";
        }
        else if (insideFunction && isReturnStatement(line)){
            output_text += "  ...does stuff...\n";
            output_text += "  " + line + "\n";
            insideFunction = false;
        }
    }

    file.close();
}
/**
 * Processes the '--file-list' command-line argument, reading file names and appending their contents to output_text.
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments array.
 * @param currentIndex A reference to the current index in the argv array.
 * @param output_text A reference to the output text string where the contents of the files will be appended.
 * @return True if processing is successful, false if an error occurs.
 */
bool processFileListArgument(int argc, char* argv[], int& currentIndex, std::string& output_text) {
    if (currentIndex + 1 < argc) {
        while (currentIndex + 1 < argc && argv[currentIndex + 1][0] != '-') {
            std::string next_arg = argv[currentIndex + 1];
            std::cout << next_arg << std::endl;

            // Open and read the file, appending its contents to output_text
            #ifdef _WIN32
            std::string dir = fs::current_path().string();
            #else
            std::string dir = fs::current_path();
            #endif
            dir+="/"+next_arg;
            
            std::ifstream file(dir);
            if (file.is_open()) {
                std::string line;
                output_text += "\nFile Name: " + next_arg + ":\n";
                while (std::getline(file, line)) {
                    output_text += line + "\n";
                }
                file.close();
            } else {
                std::cerr << "Failed to open file: " << dir << std::endl;
                return false; // Return false to indicate failure
            }
            currentIndex++;
        }
        return true; // Successfully processed all files
    } else {
        std::cerr << "Error: --file-list requires an argument" << std::endl;
        return false; // Return false to indicate failure
    }
}

/**
 * Processes command-line arguments representing file names to be processed.
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * @param currentIndex The current index in the argv array being processed.
 * @param output_text A reference to a string where the output is accumulated.
 */
void processFiles(int argc, char *argv[], int &currentIndex, std::string &output_text){
    while (currentIndex + 1 < argc && argv[currentIndex + 1][0] != '-'){
        std::string next_arg = argv[currentIndex + 1];
        processFile(next_arg, output_text);
        currentIndex++;
    }
}
/**
 * Processes the '--output' command-line argument, setting the output file name.
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments array.
 * @param currentIndex A reference to the current index in the argv array.
 * @param output_file A reference to the string where the output file name will be stored.
 * @return True if processing is successful, false if an error occurs.
 */
bool processOutputArgument(int argc, char* argv[], int& currentIndex, std::string& output_file) {
    if (currentIndex + 1 < argc) {
        std::string next_arg = argv[currentIndex + 1];
        std::cout << next_arg << std::endl;

        // Check if the next argument is not another option (doesn't start with '-')
        if (next_arg[0] != '-') {
            output_file = next_arg; // Set the output file name
            currentIndex++; // Move to the next argument since we've processed this one
            return true;
        } else {
            std::cerr << "Error: --output requires an argument and it needs to start and end with a quote" << std::endl;
        }
    } else {
        std::cerr << "Error: --output requires an argument" << std::endl;
    }
    return false; // Return false to indicate failure
}


/**
 * Processes the '--prompt' command-line argument, ensuring it has a valid following argument.
 * Appends the argument to the provided output_text string.
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments array.
 * @param currentIndex A reference to the current index in the argv array.
 * @param output_text A reference to the output text string where the argument for '--prompt' will be appended.
 * @return True if processing is successful, false if an error occurs.
 */
bool processPromptArgument(int argc, char *argv[], int &currentIndex, std::string &output_text)
{
    if (currentIndex + 1 < argc){
        std::string next_arg = argv[currentIndex + 1];
        std::cout << next_arg << std::endl;

        // Check if the next argument is not another option (doesn't start with '-')
        if (next_arg[0] != '-'){
            output_text += next_arg;
            currentIndex++; // Move to the next argument since we've processed this one
            return true;
        }
        else{
            std::cerr << "Error: --prompt requires an argument and it needs to start and end with a quote" << std::endl;
        }
    }
    else{
        std::cerr << "Error: --prompt requires an argument" << std::endl;
    }
    return false; // Return false to indicate failure
}
/**
 * The main function that processes command-line arguments and orchestrates file processing.
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * @return 0 on success, non-zero on error.
 */
int main(int argc, char *argv[])
{
    std::cout << "Number of command-line arguments: " << argc << std::endl;
    std::string output_text;
    std::string output_file;

    // Define the help string
    std::string help_string = "Usage: ./prompter [OPTION]... [FILE]...\n"
                              "Options:\n"
                              "  -h, --help     display this help and exit\n"
                              "  -V, --version  output version information and exit\n"
                              "  --file-list    read the text from the file list\n"
                              "  --function-names-from-file read the function names from the file\n"
                              "  --output       output the result to the file\n";

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h"){
            std::cout << help_string << std::endl;
            return 0;
        }
        else if (arg == "--version" || arg == "-V"){
            std::cout << "Version: 1.0" << std::endl;
            return 0;
        }
        else if (arg == "--prompt")
        {
            if (!processPromptArgument(argc, argv, i, output_text))
            {
                return 1; // If processing the prompt argument fails, exit with an error code
            }
        }else if (arg == "--output"){
            if (!processOutputArgument(argc, argv, i, output_file)) {
                return 1; // If processing the output argument fails, exit with an error code
            }
        }else if (arg == "--file-list"){
            if (!processFileListArgument(argc, argv, i, output_text)) {
                return 1; // If processing the file list argument fails, exit with an error code
            }
        }else if (arg == "--function-names-from-file"){

            if (i + 1 < argc){
                processFiles(argc, argv, i, output_text);
            }
            else{
                std::cout << "Error: --function-names-from-file requires an argument" << std::endl;
                return 1;
            }
        }else if (arg=="--dir"){
            #ifdef _WIN32
            std::string dir = fs::current_path().string();
            #else
            std::string dir = fs::current_path();
            #endif
            std::cout << dir << std::endl;
            return 0;
        }
    }

    if (output_text == "")
    {
        std::cout << "Error: Output String is empty" << std::endl;
        return 1;
    }
    else
    {
        if (output_file != "")
        {
            std::ofstream file;
            #ifdef _WIN32
            std::string dir = fs::current_path().string();
            #else
            std::string dir = fs::current_path();
            #endif
            output_file = dir + "/" + output_file;
            file.open(output_file, std::ios::in);
            file << output_text;
            file.close();
        }
        else
        {
            copyToClipboard(output_text);
            std::cout << output_text << std::endl;
        }
    }
    return 0;
}
