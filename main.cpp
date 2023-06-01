#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <windows.h>

using namespace std;

void write_json(const json& data, const string& filepath)
{
    ofstream f(filepath);
    f << data.dump(4);
    f.close();
}

json read_json(const string& filepath)
{
    json data;

    ifstream f(filepath);
    if (!f.is_open())
    {
        return data;
    }

    stringstream buffer;
    buffer << f.rdbuf();

    f.close();

    try
    {
        data = json::parse(buffer.str());
    }
    catch (exception& ex)
    {
        cerr << "Error reading JSON from file: " << ex.what() << endl;
    }

    return data;
}

void set_config(const string& config_name)
{
    json data;
    write_json(data, config_name);
    cout << "已在当前目录下创建并初始化 " << config_name << " 文件。" << endl;
}

int run_program(const string& name, const json& data)
{
    if (data.count(name))
    {
        auto program = data[name];
        auto path = program["path"].get<string>();
        auto params = program["params"].get<vector<string>>();

        stringstream ss;
        ss << "start " << path << " ";
        for (auto& param : params)
        {
            ss << param << " ";
        }

        system(ss.str().c_str());

        return 1;
    }
    else
    {
        cout << "程序不存在！" << endl;
        return 0;
    }
}

json add_program(const string& name, const string& path, const vector<string>& params, const json& data)
{
    json new_data = data;
    new_data[name] = {
      {"path", path},
      {"params", params}
    };
    return new_data;
}

json del_program(const string& name, const json& data)
{
    json new_data = data;
    if (new_data.erase(name))
    {
        cout << "程序删除成功！" << endl;
    }
    else
    {
        cout << "程序不存在！" << endl;
    }
    return new_data;
}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        cout << "请使用'" << argv[0] << " help'获取帮助。" << endl;
        return 0;
    }

    if (string(argv[1]) == "help")
    {
        cout << "使用方法：" << argv[0] << " [keyword] <parameters>" << endl
             << "关键字：" << endl
             << "  add：添加一个程序到快速启动" << endl
             << "    使用方法：" << argv[0] << " add program_name program_path program_params..." << endl
             << "  del：删除一个快速启动程序" << endl
             << "    使用方法：" << argv[0] << " del program_name" << endl
             << "  run：通过 program.json 文件运行一个程序" << endl
             << "    使用方法：" << argv[0] << " run program_name" << endl
             << "  list：列出所有快速启动的程序" << endl
             << "    使用方法：" << argv[0] << " list" << endl
             << "  reset：重置 program.json 文件并删除所有程序" << endl
             << "    使用方法：" << argv[0] << " reset" << endl
             << "  --set-config：创建或修改指定的 json 文件" << endl
             << "    使用方法：" << argv[0] << " --set-config filename.json" << endl;
        return 0;
    }

    if (string(argv[1]) == "--set-config")
    {
        string config_name = argv[2];
        if (config_name.substr(config_name.length() - 5) != ".json")
        {
            config_name += ".json";
        }
        set_config(config_name);
        return 0;
    }

    json data = read_json("program.json");

    if (string(argv[1]) == "add")
    {
        string program_name = argv[2];
        string program_path = argv[3];

        vector<string> program_params;
        for (int i = 4; i < argc; i++)
        {
            program_params.push_back(argv[i]);
        }

        data = add_program(program_name, program_path, program_params, data);
        write_json(data, "program.json");
        cout << "程序添加成功！" << endl;
    }
    else if (string(argv[1]) == "del")
    {
        string program_name = argv[2];
        data = del_program(program_name, data);
        write_json(data, "program.json");
    }
    else if (string(argv[1]) == "run")
    {
        string program_name = argv[2];
        run_program(program_name, data);
    }
    else if (string(argv[1]) == "list")
    {
        if (data.empty())
        {
            cout << "程序列表为空！" << endl;
        }
        else
        {
            for (auto& [program_name, program_info] : data.items())
            {
                auto program_path = program_info["path"].get<string>();
                auto program_params = program_info["params"].get<vector<string>>();

                cout << program_name << ": " << program_path << " ";
                for (auto& param : program_params)
                {
                    cout << param << " ";
                }
                cout << endl;
            }
        }
    }
    else if (string(argv[1]) == "reset")
    {
        data = json();
        write_json(data, "program.json");
        cout << "菜单重置成功！" << endl;
    }
    else
    {
        cout << "请使用'" << argv[0] << " help'获取帮助。" << endl;
    }

    return 0;
}
