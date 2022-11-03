#include <string>
#include <fstream>
#include <filesystem>

int main()
{
  // std::string logdir = "./logs";
  // std::string mkdirs = "mkdir -p " + logdir;
  // system(mkdirs.c_str());

  // auto logfile = logdir + "/log.txt";
  // std::ofstream file;
  // file.open(logfile.c_str(), std::ios_base::app);
  // file.close();

  std::filesystem::create_directory("./log1");
}
