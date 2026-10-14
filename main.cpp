#include "src/HttpServer.hpp"

#include <map>
#include <optional>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Task {
    int id;
    std::string title;
    bool completed;
};

std::map<int, Task> taskDB;
int nextTaskId = 1;

HttpResponse jsonResponse(
    const HttpRequest& request,
    int status,
    const std::string& reason,
    const json& data
) {
    HttpResponse response;
    response.protocol = request.version;
    response.statusCode = static_cast<decltype(response.statusCode)>(status);
    response.reasonPhase = reason;
    response.contentType = "application/json";
    response.body = data.dump();

    return response;
}

json taskToJson(const Task& task) {
    return {
        {"id", task.id},
        {"title", task.title},
        {"completed", task.completed}
    };
}

std::optional<int> taskIdFromPath(const std::string& path) {
    const std::string prefix = "/tasks/";
    if (path.compare(0, prefix.size(), prefix) != 0) return std::nullopt;

    const std::string idText = path.substr(prefix.size());
    if (idText.empty()) return std::nullopt;

    try {
        size_t parsedLength = 0;
        int id = std::stoi(idText, &parsedLength);
        if (parsedLength != idText.size()) return std::nullopt;
        return id;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

HttpResponse getTasks(const HttpRequest& request)
{
    json tasks = json::array();

    for (const auto& [id, task] : taskDB) {
        tasks.push_back(taskToJson(task));
    }

    return jsonResponse(request, 200, "OK", tasks);
}

HttpResponse getTask(const HttpRequest& request)
{
    std::optional<int> id = taskIdFromPath(request.path);
    if (!id) {
        return jsonResponse(request, 400, "Bad Request", {{"error", "Task ID must be an integer"}});
    }

    auto task = taskDB.find(*id);
    if (task == taskDB.end()) {
        return jsonResponse(request, 404, "Not Found", {{"error", "Task not found"}});
    }

    return jsonResponse(request, 200, "OK", taskToJson(task->second));
}

HttpResponse createTask(const HttpRequest& request)
{
    json data = json::parse(request.body, nullptr, false);

    if (data.is_discarded() ||
        !data.is_object() ||
        !data.contains("title") ||
        !data["title"].is_string()) {

        return jsonResponse(
            request, 400, "Bad Request",
            {{"error", "A JSON object with a string title is required"}}
        );
    }

    Task task{
        nextTaskId++,
        data["title"].get<std::string>(),
        false
    };

    taskDB[task.id] = task;

    return jsonResponse(request, 201, "Created", taskToJson(task));
}

HttpResponse updateTask(const HttpRequest& request)
{
    std::optional<int> id = taskIdFromPath(request.path);
    if (!id) {
        return jsonResponse(request, 400, "Bad Request", {{"error", "Task ID must be an integer"}});
    }

    auto task = taskDB.find(*id);
    if (task == taskDB.end()) {
        return jsonResponse(request, 404, "Not Found", {{"error", "Task not found"}});
    }

    json data = json::parse(request.body, nullptr, false);
    if (!data.is_object() || (!data.contains("title") && !data.contains("completed"))) {
        return jsonResponse(request, 400, "Bad Request", {{"error", "Provide title and/or completed"}});
    }
    if (data.contains("title") && !data["title"].is_string()) {
        return jsonResponse(request, 400, "Bad Request", {{"error", "title must be a string"}});
    }
    if (data.contains("completed") && !data["completed"].is_boolean()) {
        return jsonResponse(request, 400, "Bad Request", {{"error", "completed must be a boolean"}});
    }

    if (data.contains("title")) task->second.title = data["title"].get<std::string>();
    if (data.contains("completed")) task->second.completed = data["completed"].get<bool>();

    return jsonResponse(request, 200, "OK", taskToJson(task->second));
}

HttpResponse deleteTask(const HttpRequest& request)
{
    std::optional<int> id = taskIdFromPath(request.path);
    if (!id) {
        return jsonResponse(request, 400, "Bad Request", {{"error", "Task ID must be an integer"}});
    }

    if (taskDB.erase(*id) == 0) {
        return jsonResponse(request, 404, "Not Found", {{"error", "Task not found"}});
    }

    return jsonResponse(request, 200, "OK", {{"deleted", *id}});
}

int main()
{
    HttpServer server;

    server.GET("/tasks", getTasks);
    server.GET("/tasks/{id}", getTask);
    server.POST("/tasks", createTask);
    server.PUT("/tasks/{id}", updateTask);
    server.DELETE("/tasks/{id}", deleteTask);

    server.run(8080);
}
