/*
 * cpp-init - a small tool to initialize C++ projects
 * 
 * Copyright (c) 2021 Jonas Rathert
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "tmpl_tools.h"

#include "tmpl_main.h"
#include "tmpl_make.h"
#include "tmpl_launch.h"
#include "tmpl_tasks.h"
#include "tmpl_cpp_prop.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>

int write_tmpl_file(const std::string& fname, const char* tmpl, const std::map<std::string, std::string>& values);

/*
 * GLOBAL variable (ouch!) holding the template strings
 */
std::map<std::string, std::string> tmpl_values {
    { "%%MAINPROG%%", "helloworld" },
    { "%%CXX%%", "/usr/bin/g++-10" },
    { "%%GDB%%", "/usr/bin/gdb" },
    { "%%STD_VERSION%%", "c++20" }
};

/*
 * Function to dump the project files into a specified directory. If a file or dir with 
 * that name exists, does nothing
 */
int dump(const std::string& dir) {

    bool dir_has_trailing_slash = (dir.length() > 1 && (dir.at(dir.length()-1) == '/'));
    const std::string& basedir = dir_has_trailing_slash ? dir.substr(0, dir.length()-1) : dir;

    struct stat info;
    if ( stat(basedir.c_str(), &info) == 0 ) {  
        std::cout << "Target '" << basedir << "' exists - doing nothing" << std::endl;
        return 1;
    }
    else {
        std::cout << "Creating project in '" << basedir << "'" << std::endl;

        std::string srcdir = basedir + "/src";
        std::string vscdir = basedir + "/.vscode";
        mkdir(basedir.c_str(), 0775);
        mkdir(srcdir.c_str(), 0755);
        mkdir(vscdir.c_str(), 0755);

        std::string target = tmpl_values.at("%%MAINPROG%%");

        write_tmpl_file(srcdir + "/" + target + ".cpp", tmpl_main_str, tmpl_values);
        write_tmpl_file(basedir + "/" + "Makefile", tmpl_makefile_str, tmpl_values);
        write_tmpl_file(vscdir + "/launch.json", tmpl_launch_str, tmpl_values);
        write_tmpl_file(vscdir + "/tasks.json", tmpl_tasks_str, tmpl_values);
        write_tmpl_file(vscdir + "/c_cpp_properties.json", tmpl_cpp_prop_str, tmpl_values);
        
        return 0;
    }
}

/* 
 * Helper function to replace template strings, specified by "%%...%%" 
 */
int replace_tmpl_line(std::string& line, const std::map<std::string, std::string>& values) {
    int cnt = 0;
    size_t last = 0;
    size_t p = 0;

    while ( (p = line.find("%%", last)) != std::string::npos ) {
        last = line.find("%%", p+2) + 2;
        if (last != std::string::npos) {
            size_t len = last-p;
            const std::string& item = line.substr(p, len);
            try {
                const std::string& val = values.at(item);
                size_t nlen = val.length();
                line.replace(p, len, val);
                last += (nlen-len);
                ++cnt;
            }
            catch (std::out_of_range& e) {
                // ignore
            }
        }
    }
    return cnt;
}

/*
 * Helper function to write a specific template file
 */
int write_tmpl_file(const std::string& fname, const char* tmpl, const std::map<std::string, std::string>& values) {

    std::ofstream ofs {fname};
    if (!ofs) {
        std::cerr << "cannot open filename '" << fname << "'" << std::endl;
        return -1;
    }

    std::istringstream istr { tmpl };
    std::string line;
    int cnt = 0;
    while (std::getline(istr, line)) {
        cnt += replace_tmpl_line(line, values);
        ofs << line << "\n";
    }

    ofs.close();
    return cnt;
}

// EOF