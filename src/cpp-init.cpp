/*
 * cpp-init - a small tool to initialize C++ projects
 *
 * Program creates a small file structure containing a C++ source file with a main() method,
 * a Makefile as well as a .vscode directory to allow for easy usage with Microsofts VS Code.
 * This is by no means a full-fledged environment, and it is not very flexible, but allows
 * for a super-quick setup of a C++ project.
 * 
 * See usage info in source code
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

#include <iostream>
#include <vector>
#include <string>

#include "tmpl_tools.h"

using namespace std;

/*
 * Small utility class to read/process command line arguments
 */
class CommandArguments {
public:
    CommandArguments(int argc, char* argv[]) : args(&argv[0], &argv[argc]) {}
    
    bool optionExists(const std::string& option) const {
        return std::find(args.begin(), args.end(), option) != args.end();
    }

    std::string namedOption(const std::string& option) const {
        auto it = std::find(args.begin(), args.end(), option);
        if (it != args.end() && ++it != args.end()) {
            return *it;
        }
        else {
            return "";
        }
    }

    const std::string& operator[](int index) const { return args[index]; }

private:
    std::vector<std::string> args;
};

/*
 * Sets a template value from a comman line option (if it exists)
 */ 
void setFromOption(const CommandArguments& ca, const std::string& option, const std::string& key) {
    std::string val = ca.namedOption(option);
    if (val != "") {
        tmpl_values[key] = val;
    }
} 

/*
 * Main function
 */ 
int main(int argc, char* argv[])
{
    // simple command line argument parser
    CommandArguments args(argc, argv);

    if (argc < 2 || args[1] == "-h" || args[1] == "--help") {
        std::cout << "\ncpp-init - creates a small C++ project to be used with make and MS VS Code\n\n";
        std::cout << "usage: cpp-init progname [-d dir] [-c++ /path/to/c++-compiler] [-gdb /path/to/gdb] [-std version]\n";
        std::cout << "   progname    - name of executable to be build, main C++ file will get same name\n";
        std::cout << "   -d dir      - name of directory to be created, defaults to './<progname>'\n";
        std::cout << "   -c++ ...    - path to C++ compiler executable, defaults to '" << tmpl_values.at("%%CXX%%") << "'\n";
        std::cout << "   -gdb ...    - path to GNU debugger executable, defaults to '" << tmpl_values.at("%%GDB%%") << "'\n";
        std::cout << "   -std ...    - which C++ language standard to use, defaults to '" << tmpl_values.at("%%STD_VERSION%%") << "'\n";
        std::cout << std::endl;
        std::exit(0);
    }

    // check and replace template vallue, if they exists
    setFromOption(args, args[0], "%%MAINPROG%%");
    setFromOption(args, "-c++", "%%CXX%%");
    setFromOption(args, "-gdb", "%%GDB%%");
    setFromOption(args, "-std", "%%STD_VERSION%%");

    // identify where to output the project files
    std::string baseDir = args.namedOption("-d");
    if (baseDir == "")
        baseDir = "./" + args[1];

    // do it!   
    return dump(baseDir);
}

// EOF