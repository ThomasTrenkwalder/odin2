#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#define DBG(text) std::cout << text << std::endl

std::string mask_string(const std::string &p_content, const std::string &p_mask)
{
    size_t end = p_content.size();
    end = p_mask.size() < end ? p_mask.size() : end;

    std::string ret;

    for (int pos = 0; pos < end; ++pos)
    {
        if (p_mask[pos] != ' ')
        {
            ret += p_content[pos];
        }
    }

    return ret;
}

int main()
{
    std::ifstream ifs("/home/fred/odin2/Builds/LinuxMakefile/compile_warnings.log");

    std::string line;

    std::vector<std::string> edited_values;

    while (std::getline(ifs, line))
    {
        if (line.find("[-Wconversion]") != std::string::npos)
        {

            auto file_name = line.substr(0, line.find(":"));
            auto rest = line.substr(line.find(":") + 1);
            //DBG(rest);
            auto file_line = rest.substr(0, rest.find(":"));

            std::string type;
            if (line.find("to ‘float’") != std::string::npos)
            {
                type = "(float)";
            }
            else if (line.find("to ‘double’") != std::string::npos)
            {
                type = "(double)";
            }
            else if (line.find("to ‘int’") != std::string::npos)
            {
                type = "(int)";
            }
            else if (line.find("to ‘unsigned int’") != std::string::npos)
            {
                type = "(unsigned int)";
            }
            else
            {
                DBG("ERROR IN LINE: " << line);
                return 1;
            }

            std::string code_line;
            std::string squiggels_line;

            std::getline(ifs, code_line);
            std::getline(ifs, squiggels_line);

            auto problem_string = mask_string(code_line, squiggels_line);
            auto solution_string = type + "(" + problem_string + ")";

            //DBG(line);
            //DBG(file_name);
            //DBG(file_line);
            //DBG(code_line);
            //DBG(squiggels_line);
            //DBG(problem_string);

            auto last_slash_in_filename = file_name.find_last_of("/") + 1;
            std::string pure_file_name = file_name.substr(last_slash_in_filename, file_name.size() - last_slash_in_filename);
            auto combo = pure_file_name + file_line;

            if (std::find(edited_values.begin(), edited_values.end(), combo) != edited_values.end())
            {
                continue;
            }
            else
            {
                edited_values.push_back(combo);
            }

            //if(file_name.find("GlobalIncludes.h") != std::string::npos) {
            //    DBG(combo);
            //}

            if (problem_string.size() != 1) //weird case where only "(" would be used
            {
                std::string sed_line = "sed -i '" + file_line + "s@" + problem_string + "@" + solution_string + "@' " + file_name;

                DBG(sed_line);
            }
        }
    }
}