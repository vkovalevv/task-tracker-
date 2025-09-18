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
#include <map>
#include <algorithm>
using std::string;
using std::cout;
using std::cin;
using std::endl;

using json = nlohmann::json;

enum class TaskStatus{todo,in_progress,done};

NLOHMANN_JSON_SERIALIZE_ENUM(TaskStatus,{
    {TaskStatus::todo,"todo"},
    {TaskStatus::in_progress,"in_progress"},
    {TaskStatus::done,"done"}
});

enum class Operations{ADD,DELETE,UPDATE,MARK_IN_PROGRESS, MARKDONE, LIST};

std::map<string,Operations> commandMap = {{"add",Operations::ADD},
    {"delete",Operations::DELETE},{"update",Operations::UPDATE},
    {"mark-in-progress",Operations::MARK_IN_PROGRESS},{"markdone",Operations::MARKDONE},
    {"list",Operations::LIST}};



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
class TaskManager{
public:
    TaskManager(){
        std::ifstream input_file("tasks.json");
        ParsingJson(input_file, tasks_);
    }
    
    void AddTask(const string& description){
        size_t cur_id;
        if(!tasks_.empty()){
            json last_elem = tasks_.back();
            cur_id = static_cast<size_t>(last_elem["id"]) + 1;
        }else{
            cur_id = 1;
        }
        tasks_.push_back(Task{cur_id,description});
    }
    void UpdateTask(size_t id_update,const string& description){
        auto pos = find_if(tasks_.begin(),tasks_.end(),[id_update](const json& task){ return static_cast<size_t>(task["id"]) == id_update;});
        (*pos)["description"] = description;
    }
    void DeleteTask(size_t id){
        auto pos = find_if(tasks_.begin(),tasks_.end(),[id](const json& task){ return static_cast<size_t>(task["id"]) == id;});
        if(pos != tasks_.end()){
            tasks_.erase(pos);
        }else{
            cout << "Incorrect index" << endl;
        }
    }
    void MarkInProgress(size_t id){
        auto pos = find_if(tasks_.begin(),tasks_.end(),[id](const json& task){ return static_cast<size_t>(task["id"]) == id;});
        if(pos != tasks_.end()){
            (*pos)["status"] = TaskStatus::in_progress;
        }else{
            cout << "Incorrect index" << endl;
        }
    }
    void MarkDone(size_t id){
        auto pos = find_if(tasks_.begin(),tasks_.end(),[id](const json& task){ return static_cast<size_t>(task["id"]) == id;});
        if(pos != tasks_.end()){
            (*pos)["status"] = TaskStatus::done;
        }else{
            cout << "Incorrect index" << endl;
        }
    }
    
    void List(){
        for(auto& x: tasks_){
            cout << x.dump(2);
        }
    }
   
    void List(TaskStatus status){
        for(auto& x: tasks_){
            if(x["status"] == status)
                cout << x.dump(2);
        }
    }
    
    ~TaskManager(){
        std::ofstream output_file("tasks.json");
        output_file << tasks_.dump(2);
    }
private:
    json tasks_;
};
int main(int argc, const char * argv[]) {
    
    if(argc > 1){
        std::string_view operation = argv[1];
        Operations command;
        try{
            command = commandMap.at(operation.data());
        }catch(std::out_of_range& e){
            cout << "Incorrect key" << endl;
            throw;
        }
        TaskManager task_manager;
        switch (command) {
            case Operations::ADD:{
                task_manager.AddTask(argv[2]);
                break;
            }
            case Operations::UPDATE:{
                size_t id_update  = std::stoull(argv[2]);
                task_manager.UpdateTask(id_update, argv[3]);
                break;
                
            }
            case Operations::DELETE:{
                size_t id_delete  = std::stoull(argv[2]);
                task_manager.DeleteTask(id_delete);
                break;
            }
            case Operations::MARK_IN_PROGRESS:{
                size_t id_mark = std::stoull(argv[2]);
                task_manager.MarkInProgress(id_mark);
                break;
            }
            case Operations::MARKDONE:{
                size_t id_mark = std::stoull(argv[2]);
                task_manager.MarkDone(id_mark);
                break;
            }
            case Operations::LIST:{
                if(argc == 2){
                    task_manager.List();
                }else{
                    json operation = argv[2];
                    task_manager.List(operation);
                }
                break;
            }
            default:
                break;
        }
    }
    cout << argc;
    return 0;
}
