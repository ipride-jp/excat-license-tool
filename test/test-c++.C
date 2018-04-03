/*
 * @(#)test-c++.C
 * $Id$
 */

#include <iostream>
#include <string>
#include <vector>

#include "license.h"


int mainpp(const std::vector<std::string>& args)
{
  if (args.size() < 2) {
    std::cerr << "usage: test-main license_file application_version" << std::endl;
    std::cerr << "\tlicense_file: name of PEM encoded license file" << std::endl;
    std::cerr << "\tapplication_version: name and application version in which the license file contains." << std::endl;
    return -1;
  }

  int r = license_is_valid_file(args[1].c_str(), args[2].c_str());
  std::cout << args[1] << " is" << (r ? "" : "NOT") << " valid license file for" << args[2] << "." << std::endl;

  return r ? 0 : -2;
}


int main(int argc, char* argv[])
{
  std::vector<std::string> args;

  for (int i=0; i<argc; i++) {
    args.push_back(static_cast<std::string>(argv[i]));
  }

  return mainpp(args);
}

/* the end of file */
