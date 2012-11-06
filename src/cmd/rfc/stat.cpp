#include "cmd/rfc/stat.hpp"
#include "cfg/get.hpp"
#include "main.hpp"
#include "stats/util.hpp"

namespace cmd { namespace rfc
{

cmd::Result STATCommand::Execute()
{
  if (args.size() == 1)
  {
    control.PartReply(ftp::SystemStatus, programFullname + " status:");
    control.PartReply("< Insert status info here >");
    control.Reply("End of status.");
    return cmd::Result::Okay;
  }

  std::string options;
  std::string::size_type optOffset = 0;
  if (args[1][0] == '-')
  {
    options = args[1].substr(1);
    optOffset += args[1].length();
  }
  
  std::string path(argStr, optOffset);
  boost::trim(path);
  
  const cfg::Config& config = cfg::Get();
  std::string forcedOptions = "l" + config.Lslong().Options();
    
  control.PartReply(ftp::DirectoryStatus, "Status of " + path + ":");
  DirectoryList dirList(client, control, path, ListOptions(options, forcedOptions),
                        config.Lslong().MaxRecursion());
  
  boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();
  dirList.Execute();
  boost::posix_time::ptime end = boost::posix_time::microsec_clock::local_time();
  
  control.Reply(ftp::DirectoryStatus, "End of status (" + 
      stats::util::HighResSecondsString(start, end) + ")"); 
  return cmd::Result::Okay;
}

} /* rfc namespace */
} /* cmd namespace */
