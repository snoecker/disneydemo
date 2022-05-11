#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

void parseTitles(json::const_reference items)
{
    //Title could be collection, series, or movie
    for (const auto& item : items)  
        if (item.contains("/collectionId"_json_pointer))
         std::cout << item.at("/text/title/full/collection/default/content"_json_pointer) << std::endl; //collection
        else if (item.at("/encodedSeriesId"_json_pointer) != nullptr)
         std::cout << item.at("/text/title/full/series/default/content"_json_pointer) << std::endl; //series
        else
         std::cout << item.at("/text/title/full/program/default/content"_json_pointer) << std::endl; //movie
    return ;
}

int main(int argc, char** argv) {
    //Reference sets URL base name
    constexpr auto refUrl = "https://cd-static.bamgrid.com/dp-117731241344/sets/";

    //cURL the main URL and parse it
    cpr::Response home = cpr::Get(cpr::Url{"https://cd-static.bamgrid.com/dp-117731241344/home.json"});
    auto homeJson = json::parse(home.text);
    
    //set starting point to find items in the JSON
    auto dataStart = homeJson["data"]["StandardCollection"]["containers"]; //different way to express JSON path
    auto counter = 0;

    //Ask to dump everything or just one row of selection
    auto itemChoice = 'X';
    do{
        std::cout << "Dump everything (d) or Choose one row(c) ";
        std::cin >> itemChoice;
    }
    while (itemChoice !='d' && itemChoice !='c');
    
    int choice = 0;

    //Provide menu to pick row if that was chosen
    if (itemChoice=='c')
    {
        //print intial screen rows
        for (const auto& item : dataStart)  
            std::cout << counter++ << ") " << item.at("/set/text/title/full/set/default/content"_json_pointer) << std::endl;
    
            std::cout << "Enter a choice (0-" << --counter << ") to see items in that row ";

            std::cin >> choice;
    }
    

    //get titles!
    counter = 0;
    for (const auto& item : dataStart)  
    {
        //Are we looking for a certain row?
        if (itemChoice == 'c'  && choice != counter)
        {
            counter++;
            continue;
        }
        std::cout << std::endl << counter << ") " << item.at("/set/text/title/full/set/default/content"_json_pointer) << std::endl;

        //See if item contains refId, if so we need to read another URL
        auto startingPoint = "/" + std::to_string(counter) + "/set/refId"; //build name
        json::json_pointer refId(startingPoint);
        if (dataStart.contains(refId))
        {
            //read other JSON URL for more data
            //Build dynamic URL
            std::string ss = dataStart.at(refId);   
            auto refString = refUrl + ss + ".json";

            //cURL it
            cpr::Response ref = cpr::Get(cpr::Url{refString});
            auto refJson = json::parse(ref.text);
            json refStart;
            
            //these reference JSONs can be of different Set names. Set the pointer accordinigly 
            if (refJson.contains("/data/PersonalizedCuratedSet"_json_pointer)) 
                refStart = refJson["data"]["PersonalizedCuratedSet"];
            else if (refJson.contains("/data/CuratedSet"_json_pointer)) 
                refStart = refJson["data"]["CuratedSet"];
            else
                refStart = refJson["data"]["TrendingSet"];

            parseTitles(refStart.at("/items"_json_pointer));
        }
        else   //No additional data to read
        {
            //probably a better way to do this
            startingPoint = "/" + std::to_string(counter) + "/set/items";
            json::json_pointer listStart(startingPoint);
            parseTitles(dataStart.at(listStart));
        }
        counter++;
    }
  return 0;
}