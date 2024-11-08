#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

class Settings
{

    std::vector<std::string> fields = {
        "dt",
        "collisionDistance",
        "cameraSpeed",
        "zoomFactor",
        "numOfParticles",
        "energyAbsFactor",
        "initialMass"
        
    };


    std::unordered_map<std::string, float> settingsMap;

public:
    bool loadSettings(const std::string &filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Failed to open settings file.\n";
            return false;
        }

        std::string line;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string key;
            if (std::getline(iss, key, '='))
            {
                std::string valueStr;
                if (std::getline(iss, valueStr))
                {
                    try
                    {
                        float value = std::stof(valueStr);
                        settingsMap[key] = value;
                    }
                    catch (const std::invalid_argument &e)
                    {
                        std::cerr << "Invalid value for " << key << ": " << valueStr << "\n";
                        return false;
                    }
                }
            }
        }

        return true;
    }

    float get(std::string value)
    {
        return settingsMap.at(value);
    }

    void print_fields()
    {
        for (auto field : fields)
        {
            std::cout << field << ": " << get(field) << "\n";
        }
    }
};
#endif // __SETTINGS_H__