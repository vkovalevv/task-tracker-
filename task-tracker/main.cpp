//
//  main.cpp
//  task-tracker
//
//  Created by Всеволод Ковалев on 13.09.2025.
//
/*
 format json task record:
 {"id": _, "description": "_", "status": "_", "createdAt": "_", "updatedAt"}
 */
#define COMMAND argv(1)

#include <iostream>
#include <ctime>
#include <string>
#include <fstream>
#include "json.hpp"
#include <memory>
#include <filesystem>
#include <map>

using std::string;
using std::cout;
using std::cin;
using std::endl;

using json = nlohmann::json;

enum class TaskStatus{todo,in_progress,done};
enum class Operations{ADD,DELETE,UPDATE,MARK_IN_PROGRESS, MARKDONE, LIST};

std::map<string,Operations> commandMap = {{"add",Operations::ADD},
    {"delete",Operations::DELETE},{"update",Operations::UPDATE},
    {"mark-in-progress",Operations::MARK_IN_PROGRESS},{"markdone",Operations::MARKDONE},
    {"list",Operations::LIST}};

NLOHMANN_JSON_SERIALIZE_ENUM(TaskStatus,{
    {TaskStatus::todo,"todo"},
    {TaskStatus::in_progress,"in_progress"},
    {TaskStatus::done,"done"}
});

string CurrentTime(){
    time_t timestamp;
    time(&timestamp);
    return ctime(&timestamp);
}

struct Task{
    size_t id;
    string description;
    TaskStatus status = TaskStatus::todo;
    const string createdAt = CurrentTime();
    string updatedAt = CurrentTime();
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Task,id,description,status,createdAt,updatedAt);
};


void Test1(){
    cout << "Print time:" << endl;
    cout << CurrentTime() << endl;
    
    Task task1{1,"new task"};
    json Document(task1);
    cout << Document.dump(2);
    
    json task2 = TaskStatus::todo;
    assert(task2 == "todo");
}

void ParsingJson(std::ifstream& input_file, json& tasks){
    if(input_file.is_open()){
        input_file >> tasks;
    }else{
        tasks = json::array();
    }
}

int main(int argc, const char * argv[]) {
    Test1();
    
    if(argc > 1){
        std::string_view operation = argv[1];
        Operations command = commandMap.at(operation.data());
        
        switch (command) {
            case Operations::ADD:{
                std::ifstream input_file("tasks.json");
                json tasks = json::array();
                
                ParsingJson(input_file, tasks);
                
                size_t obj_id = tasks.size() + 1; // id current added object
                
                input_file.close(); // close the file, so that two threads dont refer to same file
                
                
                tasks.push_back(Task{obj_id,argv[2]});
                
                std::ofstream output_file("tasks.json");
                output_file << tasks.dump(2);
                break;
            }
            case Operations::UPDATE:{
                std::ifstream input_file("tasks.json");
                json tasks = json::array();
                
                ParsingJson(input_file, tasks);
                
                input_file.close();
                std::ofstream output_file("tasks.json");
                
                size_t id_update  = std::stoull(argv[2]) - 1;
                tasks.at(id_update)["description"] = argv[3];
                
                output_file << tasks.dump(2);
                break;
                
            }
            default:
                break;
        }
    }
    cout << argc;
    return 0;
}
